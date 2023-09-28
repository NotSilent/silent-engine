#include "Entity.h"
#include "MeshComponent.h"
#include "ProfillerTimer.h"
#include "Renderer.h"
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

Vertex meshVertices[]{
        Vertex(-0.5f, 0.0f, 0.5f),
        Vertex(-0.5f, 0.0f, -0.5f),
        Vertex(0.5f, 0.0f, 0.5f),
        Vertex(0.5f, 0.0f, -0.5f),
};

uint32_t meshIndices[]{0, 1, 2, 2, 1, 3};

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

    std::vector<VertexAttributeDescription> attributeDescriptions;
    attributeDescriptions.push_back(VertexAttributeDescription{
            .type = VertexAttributeType::Position,
            .format = VK_FORMAT_R32G32B32_SFLOAT,
            .stride = sizeof(Vertex),
    });

    std::vector<VertexAttribute> attributes;
    attributes.push_back(VertexAttribute{
            .description = attributeDescriptions[0],
            .buffer = VK_NULL_HANDLE,
            .bufferOffset = 0,
    });

    renderer->addBuffer("meshVertices", sizeof(meshVertices), meshVertices);
    renderer->addBuffer("meshIndices", sizeof(meshIndices), meshIndices);

    std::vector<std::shared_ptr<Texture>> textures;
    std::optional<std::shared_ptr<Material>> material= renderer->getMaterial(attributeDescriptions, {}, textures);
    if(material.has_value())
    {
        std::shared_ptr<Mesh> newMesh = std::make_shared<Mesh>(6,
                                                               renderer->getBuffer("meshVertices"),
                                                               renderer->getBuffer("meshIndices"),
                                                               attributes);

        // TODO: Recreate MeshManager
        std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>();
        meshComponent->setMesh(newMesh);
        meshComponent->setMaterial(material.value());

        std::shared_ptr<Entity> entity = std::make_shared<Entity>();
        Entity::addComponent(entity, meshComponent);

        entities.push_back(entity);
        meshComponents.push_back(meshComponent);
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

        renderer->update(drawData, timeManager->getCurrentTime(), timeManager->getDeltaTime());
    }

    return 0;
}