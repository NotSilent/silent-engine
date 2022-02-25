#include "InputSystem.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

InputSystem::InputSystem(std::shared_ptr<Window> window)
{
	_window = window;
}

void InputSystem::update()
{
	glfwPollEvents();

	// TODO: Normalize return value
    double xPos, yPos;
    glfwGetCursorPos(_window->getInternalWindow(), &xPos, &yPos);
    glfwSetCursorPos(_window->getInternalWindow(), 0.0, 0.0);

	_cursorDisplacement = {xPos, yPos};
}

KeyState InputSystem::getKeyState(Key key) const
{
	return static_cast<KeyState>(glfwGetKey(_window->getInternalWindow(), static_cast<int>(key)));
}

glm::vec2 InputSystem::getCursorDisplacement() const
{
	return _cursorDisplacement;
}
