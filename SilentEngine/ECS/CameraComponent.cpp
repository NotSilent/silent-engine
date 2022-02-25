#include "CameraComponent.h"
#include "../Camera.h"
#include "../EngineSystems/EngineStatics.h"
#include "../EngineSystems/InputSystem.h"

CameraComponent::CameraComponent() {
    _camera = std::make_shared<Camera>(EngineStatics::getAspectRatio());
    EngineStatics::setCamera(_camera);

    _inputManager = EngineStatics::getInputManager();
}

void CameraComponent::update(float deltaTime) {
    glm::vec2 input{0.0f};

    // TODO: Add deltaTime
    if (_inputManager->getKeyState(Key::W) == KeyState::Press) {
        input.y += _displacementPerSecond * deltaTime;
    }

    if (_inputManager->getKeyState(Key::S) == KeyState::Press) {
        input.y -= _displacementPerSecond * deltaTime;
    }

    if (_inputManager->getKeyState(Key::A) == KeyState::Press) {
        input.x -= _displacementPerSecond * deltaTime;
    }

    if (_inputManager->getKeyState(Key::D) == KeyState::Press) {
        input.x += _displacementPerSecond * deltaTime;
    }

    _camera->update(input, _inputManager->getCursorDisplacement() * _cameraRotationPerSecond * deltaTime);
}

std::shared_ptr<Camera> CameraComponent::getCamera() {
    return _camera;
}
