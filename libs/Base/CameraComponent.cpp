#include "CameraComponent.h"
#include "Camera.h"
#include "EngineStatics.h"
#include "InputSystem.h"

CameraComponent::CameraComponent() {
    _camera = std::make_shared<Camera>(EngineStatics::getAspectRatio());
    EngineStatics::setCamera(_camera);

    _inputManager = EngineStatics::getInputManager();
}

void CameraComponent::update(float deltaTime) {
    glm::vec3 input{0.0f};

    // TODO: Add deltaTime
    if (_inputManager->getKeyState(Key::Space) == KeyState::Press) {
        input.y += _displacementPerSecond;
    }

    if (_inputManager->getKeyState(Key::Ctrl) == KeyState::Press) {
        input.y -= _displacementPerSecond;
    }

    if (_inputManager->getKeyState(Key::A) == KeyState::Press) {
        input.x -= _displacementPerSecond;
    }

    if (_inputManager->getKeyState(Key::D) == KeyState::Press) {
        input.x += _displacementPerSecond;
    }

    if (_inputManager->getKeyState(Key::W) == KeyState::Press) {
        input.z += _displacementPerSecond;
    }

    if (_inputManager->getKeyState(Key::S) == KeyState::Press) {
        input.z -= _displacementPerSecond;
    }

    _camera->update(input, _inputManager->getCursorDisplacement() * _cameraRotationPerSecond, deltaTime);
}