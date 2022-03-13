#include "Entity.h"
#include "MeshComponent.h"
#include "ProfillerTimer.h"
#include "Renderer.h"
#include "tiny_gltf.h"
#include "DrawData.h"
#include "CameraComponent.h"
#include "EngineStatics.h"
#include "InputSystem.h"
#include "TimeSystem.h"
#include <iostream>
#include <vector>

const std::string ENGINE_NAME = "Silent Engine";

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

void OnFileSelected(const std::string &filePath, const std::shared_ptr<Renderer> &renderer,
                    std::vector<std::shared_ptr<Entity>> &entities,
                    std::vector<std::shared_ptr<MeshComponent>> &meshComponents);

int main() {
    std::shared_ptr<Window> window = std::make_shared<Window>(WIDTH, HEIGHT, ENGINE_NAME);
    std::shared_ptr<EngineStatics> engineStatics = std::make_shared<EngineStatics>(window);

    std::shared_ptr<InputSystem> inputManager = EngineStatics::getInputManager();
    std::shared_ptr<TimeManager> timeManager = EngineStatics::getTimeManager();

    // TODO: Order shouldn't matter
    std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(window);

    std::vector<std::shared_ptr<Entity>> entities;
    std::vector<std::shared_ptr<MeshComponent>> meshComponents;
    // ~Order shouldn't matter

    {
        // Add Camera
        std::shared_ptr<CameraComponent> cameraComponent = std::make_shared<CameraComponent>();
        std::shared_ptr<Entity> entityWithCamera = std::make_shared<Entity>();
        Entity::addComponent(entityWithCamera, cameraComponent);
        entities.push_back(entityWithCamera);
    }

    // TODO: move exit on escape to some component
    while (!window->shouldClose() && inputManager->getKeyState(Key::Escape) != KeyState::Press) {
        engineStatics->update();

        for (const auto &entity: entities) {
            entity->update(timeManager->getDeltaTime());
        }

        DrawData drawData(EngineStatics::getCamera());
        for (auto &meshComponent: meshComponents) {
            drawData.addDrawCall(meshComponent->getMesh(), meshComponent->getMaterial(), meshComponent->getModel());
        }

        renderer->update(drawData, timeManager->getCurrentTime(), timeManager->getDeltaTime(),
                         inputManager->HasControl(), renderer, entities, meshComponents, OnFileSelected);
    }

    return 0;
}

void OnFileSelected(const std::string &filePath, const std::shared_ptr<Renderer> &renderer,
                    std::vector<std::shared_ptr<Entity>> &entities,
                    std::vector<std::shared_ptr<MeshComponent>> &meshComponents) {
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    ProfillerTimer loadGLTFFile;
    bool result = loader.LoadASCIIFromFile(&model, &err, &warn, filePath);
    float loadGLTFduration = loadGLTFFile.end();
    std::cout << "Loading gltf file took: " << loadGLTFduration << "\n";

    ProfillerTimer createBuffers;

    uint32_t totalNumberOfPrimitives(0);
    uint32_t compatiblePrimitives(0);
    if (result) {
        for (auto &mesh: model.meshes) {
            for (auto &primitive: mesh.primitives) {
                totalNumberOfPrimitives++;
                // TODO: Variable number of attributes
                if (primitive.attributes.size() == 4) {
                    compatiblePrimitives++;

                    std::vector<VertexAttributeDescription> attributeDescriptions{4};
                    std::vector<VertexAttribute> attributes{4};

                    for (auto &attribute: primitive.attributes) {
                        tinygltf::Accessor accessor = model.accessors[attribute.second];
                        tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
                        tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

                        VertexAttributeType type = VertexAttribute::getType(attribute.first);
                        VkFormat format = VertexAttribute::getFormat(accessor.type, accessor.componentType);
                        uint32_t offset = static_cast<uint32_t>(model.bufferViews[accessor.bufferView].byteOffset);
                        uint32_t stride = VertexAttribute::getFormatSize(format);
                        std::string bufferName = buffer.uri + ".vertex." + std::to_string(accessor.bufferView);
                        unsigned char *bytes = buffer.data.data() + offset;
                        renderer->addBuffer(bufferName, static_cast<uint32_t>(bufferView.byteLength), bytes);

                        uint32_t index = 0;

                        if (type == VertexAttributeType::Position) {
                            index = 0;
                        }
                        if (type == VertexAttributeType::TexCoord0) {
                            index = 1;
                        }
                        if (type == VertexAttributeType::Normal) {
                            index = 2;
                        }
                        if (type == VertexAttributeType::Tangent) {
                            index = 3;
                        }

                        attributeDescriptions[index] = {
                                .type = type,
                                .format = format,
                                .stride = stride,
                        };

                        attributes[index] = {
                                .description = attributeDescriptions[index],
                                .buffer = renderer->getBuffer(bufferName)
                        };
                    }
                    tinygltf::Accessor accessor = model.accessors[primitive.indices];
                    tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
                    tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

                    std::string bufferName = buffer.uri + ".index." + std::to_string(accessor.bufferView);

                    uint32_t offset = static_cast<uint32_t>(model.bufferViews[accessor.bufferView].byteOffset);
                    unsigned char *bytes = buffer.data.data() + offset;
                    renderer->addBuffer(bufferName, static_cast<uint32_t>(bufferView.byteLength), bytes);

                    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(
                            static_cast<uint32_t>(model.accessors[primitive.indices].count),
                            renderer->getBuffer(bufferName),
                            attributes);

                    tinygltf::Material gltfMaterial = model.materials[primitive.material];
                    tinygltf::Texture gltfColorTexture = model.textures[gltfMaterial.pbrMetallicRoughness.baseColorTexture.index];
                    tinygltf::Texture gltfNormalTexture = model.textures[gltfMaterial.normalTexture.index];
                    tinygltf::Sampler gltfColorSampler = model.samplers[gltfColorTexture.sampler];
                    tinygltf::Sampler gltfNormalSampler = model.samplers[gltfNormalTexture.sampler];
                    tinygltf::Image gltfColorImage = model.images[gltfColorTexture.source];
                    tinygltf::Image gltfNormalImage = model.images[gltfNormalTexture.source];

                    std::string colorSamplerName =
                            buffer.uri + ".sampler." + std::to_string(gltfColorTexture.sampler);
                    renderer->addSampler(colorSamplerName);

                    std::string colorImageName = buffer.uri + ".image." + std::to_string(gltfColorTexture.source);
                    renderer->addImage(colorImageName, static_cast<uint32_t>(gltfColorImage.width),
                                       static_cast<uint32_t>(gltfColorImage.height),
                                       static_cast<uint32_t>(gltfColorImage.image.size()) * sizeof(unsigned char),
                                       gltfColorImage.image.data());

                    std::string colorTextureName =
                            buffer.uri + ".texture.color." + std::to_string(primitive.material);
                    std::shared_ptr<Sampler> colorSampler = renderer->getSampler(colorSamplerName);
                    std::shared_ptr<Image> colorImage = renderer->getImage(colorImageName);
                    renderer->addTexture(colorTextureName, colorSampler, colorImage);
                    std::shared_ptr<Texture> colorTexture = renderer->getTexture(colorTextureName);

                    //

                    std::string normalSamplerName =
                            buffer.uri + ".sampler." + std::to_string(gltfNormalTexture.sampler);
                    renderer->addSampler(normalSamplerName);

                    std::string normalImageName = buffer.uri + ".image." + std::to_string(gltfNormalTexture.source);
                    renderer->addImage(normalImageName, static_cast<uint32_t>(gltfNormalImage.width),
                                       static_cast<uint32_t>(gltfNormalImage.height),
                                       static_cast<uint32_t>(gltfNormalImage.image.size()) * sizeof(unsigned char),
                                       gltfNormalImage.image.data());

                    std::string normalTextureName =
                            buffer.uri + ".texture.normal" + std::to_string(primitive.material);
                    std::shared_ptr<Sampler> normalSampler = renderer->getSampler(normalSamplerName);
                    std::shared_ptr<Image> normalImage = renderer->getImage(normalImageName);
                    renderer->addTexture(normalTextureName, normalSampler, normalImage);
                    std::shared_ptr<Texture> normalTexture = renderer->getTexture(normalTextureName);

                    // Should be somehow related
                    std::vector<VkDescriptorType> types{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER};
                    std::vector<std::shared_ptr<Texture>> textures{colorTexture, normalTexture};
                    // ~Should be somehow related

                    auto material = renderer->getMaterial(attributeDescriptions, types, textures);

                    // TODO: Recreate MeshManager
                    std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>();
                    meshComponent->setMesh(mesh);
                    meshComponent->setMaterial(material);

                    std::shared_ptr<Entity> entity = std::make_shared<Entity>();
                    Entity::addComponent(entity, meshComponent);

                    entities.push_back(entity);
                    meshComponents.push_back(meshComponent);
                }
            }
        }
    }

    float creatingBuffersDuration = createBuffers.end();
    std::cout << "Creating buffers took: " << creatingBuffersDuration << "\n";
    std::cout << "Number of primitives:  " << totalNumberOfPrimitives << "\n";
    std::cout << "Compatible primitives: " << compatiblePrimitives << "\n";
}