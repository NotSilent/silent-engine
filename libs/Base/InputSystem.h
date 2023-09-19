#pragma once

#include <memory>
#include "Window.h"

#include "glm/glm.hpp"

enum class Key {
    A = GLFW_KEY_A,
    D = GLFW_KEY_D,
    F = GLFW_KEY_F,
    S = GLFW_KEY_S,
    W = GLFW_KEY_W,
    Escape = GLFW_KEY_ESCAPE,
    Tab = GLFW_KEY_TAB,
};

enum class KeyState {
    Press = GLFW_PRESS,
    Release = GLFW_RELEASE,
};

class InputSystem {
    friend class EngineStatics;

public:
    explicit InputSystem(std::shared_ptr<Window> window);

    void update();

    [[nodiscard]] KeyState getKeyState(Key key) const;

    [[nodiscard]] glm::vec2 getCursorDisplacement() const;

    [[nodiscard]] bool HasControl() const;

private:
    std::shared_ptr<Window> _window;
    glm::vec2 _previousPosition = glm::vec2{};
    glm::vec2 _cursorDisplacement = glm::vec2{};

    KeyState _previousTabKeyState = KeyState::Release;
    bool _hasControl = false;
};
