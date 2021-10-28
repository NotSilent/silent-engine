#include "Entity.h"
#include "MeshComponent.h"
#include "Renderer.h"
#include <DrawData.h>
#include <InputManager.h>
#include <TimeManager.h>

const std::string STANFORD_BUNNY_ASSET_LOCATION = "assets/stanford-bunny.obj";

const std::string PINK_TEXTURE_ASSET_LOCATION = "assets/pink.png";
const std::string TEST_TEXTURE_ASSET_LOCATION = "assets/test.png";

const std::string ENGINE_NAME = "Silent Engine";

const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

int main()
{
    std::shared_ptr<Window> window = std::make_shared<Window>(WIDTH, HEIGHT, ENGINE_NAME);

    // TODO: Common base for managers
    InputManager inputManager { window };
    TimeManager timeManager { window };

    Renderer renderer { window };

    Camera camera = Camera(static_cast<float>(window->getWidth()), static_cast<float>(window->getHeight()));

    std::shared_ptr<MeshComponent> meshComponent = std::make_shared<MeshComponent>();
    meshComponent->setMesh(renderer.getMesh(STANFORD_BUNNY_ASSET_LOCATION));
    meshComponent->setTexture(renderer.getTexture(TEST_TEXTURE_ASSET_LOCATION));

    Entity test;
    test.addComponent(meshComponent);

    // TODO: move exit on escape to some component
    while (!window->shouldClose() && inputManager.getKeyState(Key::Escape) != KeyState::Press) {
        timeManager.update();
        inputManager.update();

        glm::vec2 input { 0.0f };

        // TODO: Add deltaTime
        // TODO: Change Camera to CameraComponent
        // TODO: Move to CameraComponent
        if (inputManager.getKeyState(Key::W) == KeyState::Press) {
            input.y += 1.0f;
        }

        if (inputManager.getKeyState(Key::S) == KeyState::Press) {
            input.y += -1.0f;
        }

        if (inputManager.getKeyState(Key::A) == KeyState::Press) {
            input.x += -1.0f;
        }

        if (inputManager.getKeyState(Key::D) == KeyState::Press) {
            input.x += 1.0f;
        }

        camera.update(timeManager.getDeltaTime(), input, inputManager.getCursorDisplacement() * 10.0f);
        DrawData drawData(camera);
        // TODO: gather all MeshComponents from entities and add a draw call for each
        // TODO: add translation
        drawData.addDrawCall(meshComponent->getMesh(), meshComponent->getTexture());

        renderer.update(drawData, timeManager.getCurrentTime(), timeManager.getDeltaTime(), timeManager.getCurrentFrame());
    }

    return 0;
}