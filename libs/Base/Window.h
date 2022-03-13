#pragma once

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include <string>

struct GLFWwindow;

class Window {
public:
    Window(int width, int height, const std::string& name);
    ~Window();

    VkSurfaceKHR createSurface(VkInstance instance);
    void destroySurface(VkInstance instance, VkSurfaceKHR surface);

    bool shouldClose() const;

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    std::string getName() const;
    float getAspectRatio() const;

    GLFWwindow* getInternalWindow() const;

private:
    GLFWwindow* _window;
    std::string _name;
    uint32_t _width;
    uint32_t _height;
};
