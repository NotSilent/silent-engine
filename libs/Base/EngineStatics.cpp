#include "EngineStatics.h"
#include "../Base/Camera.h"
#include "InputSystem.h"
#include "TimeSystem.h"
#include "../Base/Window.h"
#include <stdexcept>
#include <utility>

std::shared_ptr<Window> EngineStatics::_window;
std::shared_ptr<Camera> EngineStatics::_camera;
std::shared_ptr<InputSystem> EngineStatics::_inputManager;
std::shared_ptr<TimeManager> EngineStatics::_timeManager;

EngineStatics::EngineStatics(const std::shared_ptr<Window> &window) {
    if (_window) {
        throw std::runtime_error("EngineManagers initialized already");
    }

    _window = window;

    _camera = std::make_shared<Camera>(16.0f / 9.0f);

    _inputManager = std::make_shared<InputSystem>(window);
    _timeManager = std::make_shared<TimeManager>(window);
}

void EngineStatics::update() {
    _inputManager->update();
    _timeManager->update();
}

float EngineStatics::getAspectRatio() {
    return _window->getAspectRatio();
}

void EngineStatics::setCamera(std::shared_ptr<Camera> camera) {
    _camera = std::move(camera);
}

std::shared_ptr<Camera> EngineStatics::getCamera() {
    return _camera;
}

std::shared_ptr<InputSystem> EngineStatics::getInputManager() {
    return _inputManager;
}

std::shared_ptr<TimeManager> EngineStatics::getTimeManager() {
    return _timeManager;
}
