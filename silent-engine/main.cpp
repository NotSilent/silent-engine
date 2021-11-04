#include "Entity.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include <CameraComponent.h>
#include <DrawData.h>
#include <EngineStatics.h>
#include <InputManager.h>
#include <TimeManager.h>
#include <vector>

const std::string STANFORD_BUNNY_ASSET_LOCATION = "assets/stanford-bunny.obj";

const std::string PINK_TEXTURE_ASSET_LOCATION = "assets/pink.png";
const std::string TEST_TEXTURE_ASSET_LOCATION = "assets/test.png";

const std::string ENGINE_NAME = "Silent Engine";

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

int main()
{
    std::shared_ptr<Window> window = std::make_shared<Window>(WIDTH, HEIGHT, ENGINE_NAME);
    std::shared_ptr<EngineStatics> engineStatics = std::make_shared<EngineStatics>(window);

    std::shared_ptr<InputManager> inputManager = EngineStatics::getInputManager();
    std::shared_ptr<TimeManager> timeManager = EngineStatics::getTimeManager();

    Renderer renderer { window };

    // TODO: Deserialize from file
    std::shared_ptr<MeshComponent> meshComponent0 = std::make_shared<MeshComponent>();
    meshComponent0->setMesh(renderer.getMesh(STANFORD_BUNNY_ASSET_LOCATION));
    meshComponent0->setTexture(renderer.getTexture(TEST_TEXTURE_ASSET_LOCATION));
    std::shared_ptr<MeshComponent> meshComponent1 = std::make_shared<MeshComponent>();
    meshComponent1->setMesh(renderer.getMesh(STANFORD_BUNNY_ASSET_LOCATION));
    meshComponent1->setTexture(renderer.getTexture(TEST_TEXTURE_ASSET_LOCATION));
    std::shared_ptr<MeshComponent> meshComponent2 = std::make_shared<MeshComponent>();
    meshComponent2->setMesh(renderer.getMesh(STANFORD_BUNNY_ASSET_LOCATION));
    meshComponent2->setTexture(renderer.getTexture(TEST_TEXTURE_ASSET_LOCATION));

    std::shared_ptr<Entity> entityWithMesh0 = std::make_shared<Entity>();
    entityWithMesh0->translate(glm::vec3(-0.5f, -0.5f, 0.0f));
    Entity::addComponent(entityWithMesh0, meshComponent0);
    std::shared_ptr<Entity> entityWithMesh1 = std::make_shared<Entity>();
    entityWithMesh1->translate(glm::vec3(0.5f, -0.5f, 0.0f));
    Entity::addComponent(entityWithMesh1, meshComponent1);
    std::shared_ptr<Entity> entityWithMesh2 = std::make_shared<Entity>();
    entityWithMesh2->translate(glm::vec3(0.0f, 0.5f, 0.0f));
    Entity::addComponent(entityWithMesh2, meshComponent2);

    std::shared_ptr<CameraComponent> cameraComponent = std::make_shared<CameraComponent>();

    std::shared_ptr<Entity> entityWithCamera = std::make_shared<Entity>();
    Entity::addComponent(entityWithCamera, cameraComponent);

    std::vector<std::shared_ptr<Entity>> entities;
    entities.push_back(entityWithMesh0);
    entities.push_back(entityWithMesh1);
    entities.push_back(entityWithMesh2);
    entities.push_back(entityWithCamera);

    // TODO: move exit on escape to some component
    while (!window->shouldClose() && inputManager->getKeyState(Key::Escape) != KeyState::Press) {
        engineStatics->update();

        for (auto entity : entities) {
            entity->update(timeManager->getDeltaTime());
        }

        DrawData drawData(EngineStatics::getCamera());
        // TODO: gather all MeshComponents from entities and add a draw call for each
        drawData.addDrawCall(meshComponent0->getMesh(), meshComponent0->getTexture(), entityWithMesh0->getModel());
        drawData.addDrawCall(meshComponent1->getMesh(), meshComponent1->getTexture(), entityWithMesh1->getModel());
        drawData.addDrawCall(meshComponent2->getMesh(), meshComponent2->getTexture(), entityWithMesh2->getModel());

        renderer.update(drawData, timeManager->getCurrentTime(), timeManager->getDeltaTime(), timeManager->getCurrentFrame());
    }

    return 0;
}