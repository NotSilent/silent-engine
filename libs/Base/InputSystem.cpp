#include "InputSystem.h"

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <utility>

InputSystem::InputSystem(std::shared_ptr<Window> window)
        : _window(std::move(window)) {
}

void InputSystem::update() {
    glfwPollEvents();

    KeyState tabKeyState = getKeyState(Key::Tab);
    if (_previousTabKeyState == KeyState::Release && tabKeyState == KeyState::Press) {
        _hasControl = !_hasControl;
        _cursorDisplacement = {0.0f, 0.0f};
        glfwSetCursorPos(_window->getInternalWindow(), 0.0, 0.0);
        if (_hasControl) {
            glfwSetInputMode(_window->getInternalWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            glfwSetInputMode(_window->getInternalWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    if (_hasControl) {
        // TODO: Normalize return value
        double xPos, yPos;
        glfwGetCursorPos(_window->getInternalWindow(), &xPos, &yPos);
        glfwSetCursorPos(_window->getInternalWindow(), 0.0, 0.0);

        _cursorDisplacement = {xPos, yPos};
    }

    _previousTabKeyState = tabKeyState;
}

KeyState InputSystem::getKeyState(Key key) const {
    return static_cast<KeyState>(glfwGetKey(_window->getInternalWindow(), static_cast<int>(key)));
}

glm::vec2 InputSystem::getCursorDisplacement() const {
    return _cursorDisplacement;
}

bool InputSystem::HasControl() const {
    return _hasControl;
}
