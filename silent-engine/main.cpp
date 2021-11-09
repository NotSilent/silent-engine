#include "Entity.h"
#include "MeshComponent.h"
#include "ProfillerTimer.h"
#include "Renderer.h"
#include "tinygltf\tiny_gltf.h"
#include <CameraComponent.h>
#include <DrawData.h>
#include <EngineStatics.h>
#include <InputManager.h>
#include <TimeManager.h>
#include <iostream>
#include <vector>

const std::string ENGINE_NAME = "Silent Engine";

const std::string SPONZA_FILENAME = "third-party/glTF-Sample-Models/2.0/Sponza/glTF/Sponza.gltf";
const std::string TEMP_SPONZA_IMAGES_LOCATION = "third-party/glTF-Sample-Models/2.0/Sponza/glTF/";

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

int main()
{
    std::shared_ptr<Window> window = std::make_shared<Window>(WIDTH, HEIGHT, ENGINE_NAME);
    std::shared_ptr<EngineStatics> engineStatics = std::make_shared<EngineStatics>(window);

    std::shared_ptr<InputManager> inputManager = EngineStatics::getInputManager();
    std::shared_ptr<TimeManager> timeManager = EngineStatics::getTimeManager();

    Renderer renderer { window };

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    std::vector<std::shared_ptr<Entity>> entities;
    int a = 0;
    std::vector<std::shared_ptr<MeshComponent>> meshComponents;

    ProfillerTimer loadGLTFFile;
    bool result = loader.LoadASCIIFromFile(&model, &err, &warn, SPONZA_FILENAME);
    float loadGLTFduration = loadGLTFFile.end();
    std::cout << "Loading gltf file took: " << loadGLTFduration << "\n";

    ProfillerTimer createBuffers;
    if (result) {
        for (auto& mesh : model.meshes) {
            for (auto& primitive : mesh.primitives) {
                // TODO: Variable number of attributes
                if (primitive.attributes.size() == 4) {
                    std::vector<VertexAttribute> attributes { 4 };

                    for (auto& attribute : primitive.attributes) {
                        tinygltf::Accessor accessor = model.accessors[attribute.second];
                        tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
                        tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

                        VertexAttributeType type = VertexAttribute::getType(attribute.first);
                        VkFormat format = VertexAttribute::getFormat(accessor.type, accessor.componentType);
                        uint32_t offset = model.bufferViews[accessor.bufferView].byteOffset;
                        uint32_t stride = VertexAttribute::getFormatSize(format);
                        std::string bufferName = buffer.uri + ".vertex." + std::to_string(accessor.bufferView);
                        unsigned char* bytes = buffer.data.data() + offset;
                        renderer.addBuffer(bufferName, bufferView.byteLength, bytes);

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

                        attributes[index] = {
                            .type = type,
                            .format = format,
                            .stride = stride,
                            .buffer = renderer.getBuffer(bufferName)
                        };
                    }
                    tinygltf::Accessor accessor = model.accessors[primitive.indices];
                    tinygltf::BufferView bufferView = model.bufferViews[accessor.bufferView];
                    tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

                    std::string bufferName = buffer.uri + ".index." + std::to_string(accessor.bufferView);

                    uint32_t offset = model.bufferViews[accessor.bufferView].byteOffset;
                    unsigned char* bytes = buffer.data.data() + offset;
                    renderer.addBuffer(bufferName, bufferView.byteLength, bytes);

                    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(
                        model.accessors[primitive.indices].count,
                        renderer.getBuffer(bufferName),
                        attributes);

                    std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>();
                    meshComponent->setMesh(mesh);

                    //meshComponent->setTexture(renderer.getTexture(TEST_TEXTURE_ASSET_LOCATION));
                    //
                    // TEMP
                    tinygltf::Material gltfMaterial = model.materials[primitive.material];
                    tinygltf::Texture gltfTexture = model.textures[gltfMaterial.pbrMetallicRoughness.baseColorTexture.index];
                    tinygltf::Sampler gltfSampler = model.samplers[gltfTexture.sampler];
                    tinygltf::Image gltfImage = model.images[gltfTexture.source];

                    std::string samplerName = buffer.uri + ".sampler." + std::to_string(gltfTexture.sampler);
                    renderer.addSampler(samplerName);

                    std::string imageName = buffer.uri + ".image." + std::to_string(gltfTexture.source);
                    renderer.addImage(imageName, gltfImage.width, gltfImage.height, gltfImage.image.size() * sizeof(unsigned char), gltfImage.image.data());

                    std::string textureName = buffer.uri + ".texture." + std::to_string(primitive.material);
                    std::shared_ptr<Sampler> sampler = renderer.getSampler(samplerName);
                    std::shared_ptr<Image> image = renderer.getImage(imageName);
                    renderer.addTexture(textureName, sampler, image);

                    std::shared_ptr<Texture> texture = renderer.getTexture(textureName);
                    meshComponent->setTexture(texture);
                    // ~TEMP

                    std::shared_ptr<Entity> entity = std::make_shared<Entity>();
                    //entity->setScale({100.0f, 100.0f, 100.0f});
                    Entity::addComponent(entity, meshComponent);

                    entities.push_back(entity);
                    meshComponents.push_back(meshComponent);

                    if (a >= 10) {
                        break;
                    }
                    a++;
                }
            }
        }
    }

    float creatingBuffersDuration = createBuffers.end();
    std::cout << "Creating buffers took: " << creatingBuffersDuration << "\n";

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

        for (auto entity : entities) {
            entity->update(timeManager->getDeltaTime());
        }

        DrawData drawData(EngineStatics::getCamera());
        // TODO: gather all MeshComponents from entities and add a draw call for each
        //drawData.addDrawCall(meshComponent0->getMesh(), meshComponent0->getTexture(), entityWithMesh0->getModel());
        //drawData.addDrawCall(meshComponent1->getMesh(), meshComponent1->getTexture(), entityWithMesh1->getModel());
        //drawData.addDrawCall(meshComponent2->getMesh(), meshComponent2->getTexture(), entityWithMesh2->getModel());

        for (auto& meshComponent : meshComponents) {
            drawData.addDrawCall(meshComponent->getMesh(), meshComponent->getTexture(), meshComponent->getModel());
        }

        renderer.update(drawData, timeManager->getCurrentTime(), timeManager->getDeltaTime(), timeManager->getCurrentFrame());
    }

    return 0;
}