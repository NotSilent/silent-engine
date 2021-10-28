#include "InputManager.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

InputManager::InputManager(std::shared_ptr<Window> window)
{
	_window = window;
}

void InputManager::update()
{
	glfwPollEvents();

	// TODO: Normalize return value
    double xPos, yPos;
    glfwGetCursorPos(_window->getInternalWindow(), &xPos, &yPos);
    glfwSetCursorPos(_window->getInternalWindow(), 0.0, 0.0);

	_cursorDisplacement = {xPos, yPos};
}

KeyState InputManager::getKeyState(Key key) const
{
	return static_cast<KeyState>(glfwGetKey(_window->getInternalWindow(), static_cast<int>(key)));
}

glm::vec2 InputManager::getCursorDisplacement() const
{
	return _cursorDisplacement;
}
