#include "Window.h"
#include <stdexcept>

Window::Window(int width, int height, const std::string& name)
{
    _width = static_cast<uint32_t>(width);
    _height = static_cast<uint32_t>(height);
    _name = name;

    if (!glfwInit()) {
        throw std::runtime_error("Error: glfwInit");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(width, height, _name.c_str(), NULL, NULL);
    if (!_window) {
        glfwTerminate();

        throw std::runtime_error("Error: glfwCreateWindow");
    }
    glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetCursorPos(_window, 0.0, 0.0);
}

Window::~Window()
{
    glfwDestroyWindow(_window);
    glfwTerminate();
}

VkSurfaceKHR Window::createSurface(VkInstance instance)
{
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(instance, _window, nullptr, &surface);

    return surface;
}

void Window::destroySurface(VkInstance instance, VkSurfaceKHR surface)
{
    vkDestroySurfaceKHR(instance, surface, nullptr);
}

bool Window::shouldClose() const
{
    return glfwWindowShouldClose(_window);
}

uint32_t Window::getWidth() const
{
    return _width;
}

uint32_t Window::getHeight() const
{
    return _height;
}

std::string Window::getName() const
{
    return _name;
}

GLFWwindow* Window::getInternalWindow() const
{
    return _window;
}
