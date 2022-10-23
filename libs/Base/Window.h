#pragma once

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"
#include <string>

class Window {
public:
    Window(int width, int height, const std::string &name);

    ~Window();

    [[nodiscard]] VkSurfaceKHR createSurface(VkInstance instance);

    void destroySurface(VkInstance instance, VkSurfaceKHR surface);

    [[nodiscard]] bool shouldClose() const;

    [[nodiscard]] uint32_t getWidth() const;

    [[nodiscard]] uint32_t getHeight() const;

    [[nodiscard]] std::string getName() const;

    [[nodiscard]] float getAspectRatio() const;

    [[nodiscard]] GLFWwindow *getInternalWindow() const;

private:
    GLFWwindow *_window;
    std::string _name;
    uint32_t _width;
    uint32_t _height;
};
