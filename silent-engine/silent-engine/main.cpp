#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "vk-bootstrap/VkBootstrap.h"

#include "VkInit.h"

const std::string ENGINE_NAME = "Silent Engine";
const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

uint64_t _currentFrame = 0;

vkb::Instance _instance;
vkb::PhysicalDevice _physicalDevice;
vkb::Device _device;
vkb::Swapchain _swapchain;

VkSurfaceKHR _surface;

std::vector<VkImage> _swapchainImages;
std::vector<VkImageView> _swapchainImageViews;
std::vector<VkFramebuffer> _swapchainFramebuffers;

VkCommandPool _commandPool;
std::vector<VkRenderPass> _renderPasses;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void init(GLFWwindow* window);
void cleanup();
void draw();

int main()
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

    init(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        draw();
    }

    cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void init(GLFWwindow* window)
{
    vkb::InstanceBuilder builder;
    auto instanceResult = builder.set_app_name(ENGINE_NAME.c_str()).request_validation_layers(true).use_default_debug_messenger().build();

    if (!instanceResult) {
        throw std::runtime_error(instanceResult.error().message());
    }

    _instance = instanceResult.value();

    glfwCreateWindowSurface(_instance.instance, window, nullptr, &_surface);

    vkb::PhysicalDeviceSelector selector { _instance };

    auto physicalDeviceResult = selector.set_surface(_surface).set_minimum_version(1, 1).select();
    if (!physicalDeviceResult) {
        throw std::runtime_error(physicalDeviceResult.error().message());
    }

    _physicalDevice = physicalDeviceResult.value();

    vkb::DeviceBuilder deviceBuilder { _physicalDevice };

    auto deviceResult = deviceBuilder.build();
    if (!deviceResult) {
        throw std::runtime_error(deviceResult.error().message());
    }

    _device = deviceResult.value();

    vkb::SwapchainBuilder swapchainBuilder { _device };

    auto swapchainResult = swapchainBuilder.use_default_format_selection().set_desired_present_mode(VK_PRESENT_MODE_FIFO_RELAXED_KHR).set_desired_extent(WIDTH, HEIGHT).build();
    if (!swapchainResult) {
        throw std::runtime_error(swapchainResult.error().message());
    }

    _swapchain = swapchainResult.value();
    _swapchainImages = _swapchain.get_images().value();
    _swapchainImageViews = _swapchain.get_image_views().value();

    _swapchainFramebuffers = std::vector<VkFramebuffer>(_swapchainImageViews.size());
    _renderPasses = std::vector<VkRenderPass>(_swapchainImageViews.size());

    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        _renderPasses[i] = VkInit::createRenderPass(_device, _swapchain, _swapchainImageViews[i]);
        _swapchainFramebuffers[i] = VkInit::createFramebuffer(_device, _renderPasses[i], _swapchainImageViews[i], WIDTH, HEIGHT);
    }

    _commandPool = VkInit::createCommandPool(_device);
}

void cleanup()
{
    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        vkDestroyFramebuffer(_device.device, _swapchainFramebuffers[i], nullptr);
        vkDestroyRenderPass(_device.device, _renderPasses[i], nullptr);
    }

    vkDestroyCommandPool(_device.device, _commandPool, nullptr);

    _swapchain.destroy_image_views(_swapchainImageViews);
    vkb::destroy_swapchain(_swapchain);

    vkDestroySurfaceKHR(_instance.instance, _surface, nullptr);

    vkb::destroy_device(_device);
    vkb::destroy_instance(_instance);
}

void draw()
{
    auto acquireSemaphore = VkInit::createSemaphore(_device);
    uint32_t imageIndex;

    if (vkAcquireNextImageKHR(_device.device, _swapchain.swapchain, std::numeric_limits<uint32_t>::max(), acquireSemaphore, nullptr, &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't acquire next image");
    }

    auto gray = [&]() {
        return abs(sinf(_currentFrame / 100.0f));
    }();

    VkClearValue clearValue { gray, gray, gray };

    VkRenderPassBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = _renderPasses[imageIndex],
        .framebuffer = _swapchainFramebuffers[imageIndex],
        .renderArea = VkRect2D { 0, 0, WIDTH, HEIGHT },
        .clearValueCount = 1,
        .pClearValues = &clearValue,
    };

    VkCommandBufferBeginInfo cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    VkCommandBufferAllocateInfo allocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = _commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(_device.device, &allocateInfo, &cmd);

    vkBeginCommandBuffer(cmd, &cmdBeginInfo);

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    const auto pipelineLayout = VkInit::Pipeline::createDefaultPipelineLayout(_device);
    const auto pipeline = VkInit::Pipeline::createDefaultPipeline(_device, pipelineLayout, _renderPasses[imageIndex], WIDTH, HEIGHT);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdDraw(cmd, 3, 1, 0, 0);

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

    auto queueFence = VkInit::createFence(_device);

    VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    auto submitSemaphore = VkInit::createSemaphore(_device);

    VkSubmitInfo submitInfo {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &acquireSemaphore,
        .pWaitDstStageMask = &waitDstStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &cmd,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &submitSemaphore,
    };

    vkQueueSubmit(_device.get_queue(vkb::QueueType::graphics).value(), 1, &submitInfo, queueFence);

    VkPresentInfoKHR presentInfo {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &submitSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &_swapchain.swapchain,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    vkQueuePresentKHR(_device.get_queue(vkb::QueueType::graphics).value(), &presentInfo);

    vkWaitForFences(_device.device, 1, &queueFence, true, std::numeric_limits<uint64_t>::max());

    vkDestroyFence(_device.device, queueFence, nullptr);
    vkDestroySemaphore(_device.device, acquireSemaphore, nullptr);
    vkDestroySemaphore(_device.device, submitSemaphore, nullptr);

    vkDestroyPipeline(_device.device, pipeline, nullptr);
    vkDestroyPipelineLayout(_device.device, pipelineLayout, nullptr);

    _currentFrame++;
}