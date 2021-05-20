#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdlib.h>

#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

int main(void)
{
    if (!glfwInit()) {
        throw std::runtime_error("Couldn't init GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Silent Engine", NULL, NULL);

    if (!window) {
        glfwTerminate();

        throw std::runtime_error("GLFWwindow creation failed");
    }

    glfwSetKeyCallback(window, key_callback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}