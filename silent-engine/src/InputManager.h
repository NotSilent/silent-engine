#pragma once

#include <memory>
#include "Window.h"
#include "glm\glm.hpp"

enum class Key {
    A = GLFW_KEY_A,
    D = GLFW_KEY_D,
    F = GLFW_KEY_F,
    S = GLFW_KEY_S,
    W = GLFW_KEY_W,
    Escape = GLFW_KEY_ESCAPE,
};

enum class KeyState {
    Press = GLFW_PRESS,
    Release = GLFW_RELEASE,
};

class InputManager {
public:
    InputManager(std::shared_ptr<Window> window);

    void update();
    KeyState getKeyState(Key key) const;
    glm::vec2 getCursorDisplacement() const;

private:
    std::shared_ptr<Window> _window;
    glm::vec2 _cursorDisplacement;
};
