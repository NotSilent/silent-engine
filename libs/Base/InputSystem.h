#pragma once

#include <memory>
#include "../Base/Window.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#include "glm\glm.hpp"

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
    glm::vec2 _cursorDisplacement = glm::vec2{};

    KeyState _previousTabKeyState = KeyState::Release;
    bool _hasControl = false;
};
