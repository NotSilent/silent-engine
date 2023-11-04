#include "Entity.h"
#include "MeshComponent.h"
#include "ProfillerTimer.h"
#include "Renderer.h"
#include "DrawData.h"
#include "CameraComponent.h"
#include "EngineStatics.h"
#include "InputSystem.h"
#include "TimeSystem.h"
#include "PatchedSphere.h"
#include <iostream>
#include <vector>

const std::string ENGINE_NAME = "Silent Engine";

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

// TODO: Plane generator
std::array planeIndices{
        0, 2, 1, 1, 2, 3,
};

std::array planePositions{
        glm::vec3{-1.0f, 1.0f, 0.0f},
        glm::vec3{1.0f, 1.0f, 0.0f},
        glm::vec3{-1.0f, -1.0f, 0.0f},
        glm::vec3{1.0f, -1.0f, 0.0f},
};

std::array planeNormals{
        glm::vec3{0.0f, 0.0f, -1.0f},
        glm::vec3{0.0f, 0.0f, -1.0f},
        glm::vec3{0.0f, 0.0f, -1.0f},
        glm::vec3{0.0f, 0.0f, -1.0f},
};

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

    PatchedSphere patchedSphere(4);

    renderer->addBuffer("sphereIndices", VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        patchedSphere.getIndices().size() * sizeof(uint32_t), patchedSphere.getIndices().data());
    renderer->addBuffer("sphereVertices", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        patchedSphere.getPositions().size() * sizeof(glm::vec3),
                        patchedSphere.getPositions().data());
    renderer->addBuffer("sphereNormals", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        patchedSphere.getNormals().size() * sizeof(glm::vec3),
                        patchedSphere.getNormals().data());

    renderer->addBuffer("planeIndices", VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        planeIndices.size() * sizeof(uint32_t), planeIndices.data());
    renderer->addBuffer("planeVertices", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        planePositions.size() * sizeof(glm::vec3),
                        planePositions.data());
    renderer->addBuffer("planeNormals", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                        planeNormals.size() * sizeof(glm::vec3),
                        planeNormals.data());

    VkPipeline pipeline = renderer->getDeferredPipeline();
    if (pipeline != nullptr) {
        std::shared_ptr<Mesh> sphereMesh = std::make_shared<Mesh>(patchedSphere.getIndices().size(),
                                                                  renderer->getBuffer("sphereIndices"),
                                                                  renderer->getBuffer("sphereVertices"),
                                                                  renderer->getBuffer("sphereNormals"));

        std::shared_ptr<Mesh> planeMesh = std::make_shared<Mesh>(planeIndices.size(),
                                                                  renderer->getBuffer("planeIndices"),
                                                                  renderer->getBuffer("planeVertices"),
                                                                  renderer->getBuffer("planeNormals"));

        // TODO: Recreate MeshManager

        std::shared_ptr<Entity> entity = std::make_shared<Entity>();

        std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>();
        meshComponent->setMesh(sphereMesh);
        meshComponent->setPipeline(pipeline);
        Entity::addComponent(entity, meshComponent);

        entities.push_back(entity);
        meshComponents.push_back(meshComponent);

        std::shared_ptr<Entity> entityTop = std::make_shared<Entity>();
        entityTop->translate(glm::vec3{0.0f, 5.0f, 0.0f});

        std::shared_ptr<MeshComponent> meshTop = std::make_shared<MeshComponent>();
        meshTop->setMesh(sphereMesh);
        meshTop->setPipeline(pipeline);
        Entity::addComponent(entityTop, meshTop);

        entities.push_back(entityTop);
        meshComponents.push_back(meshTop);

        std::shared_ptr<Entity> entityRight = std::make_shared<Entity>();
        entityRight->translate(glm::vec3{5.0f, 0.0f, 0.0f});

        std::shared_ptr<MeshComponent> meshRight = std::make_shared<MeshComponent>();
        meshRight->setMesh(sphereMesh);
        meshRight->setPipeline(pipeline);
        Entity::addComponent(entityRight, meshRight);

        entities.push_back(entityRight);
        meshComponents.push_back(meshRight);

        std::shared_ptr<Entity> entityPlane = std::make_shared<Entity>();
        entityPlane->rotate(std::numbers::pi / 4.0f, {1.0f, 0.0f, 0.0f});
        entityPlane->translate({0.0f, 2.5f, 2.5f});
        entityPlane->setScale(10.0f);

        std::shared_ptr<MeshComponent> meshPlane = std::make_shared<MeshComponent>();
        meshPlane->setMesh(planeMesh);
        meshPlane->setPipeline(pipeline);
        Entity::addComponent(entityPlane, meshPlane);

        entities.push_back(entityPlane);
        meshComponents.push_back(meshPlane);
    }

    // TODO: move exit on escape to some component
    while (!window->shouldClose() && inputManager->getKeyState(Key::Escape) != KeyState::Press) {
        engineStatics->update();

        for (const auto &entity: entities) {
            entity->update(timeManager->getDeltaTime());
        }

        DrawData drawData(EngineStatics::getCamera(), renderer->getDeferredPipelineLayout());
        for (auto &meshComponent: meshComponents) {
            drawData.addDrawCall(meshComponent->getMesh(), meshComponent->getPipeline(), meshComponent->getModel());
        }

        renderer->update(drawData, timeManager->getCurrentTime(), timeManager->getDeltaTime());
    }

    return 0;
}