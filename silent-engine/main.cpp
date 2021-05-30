#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "vk-bootstrap/VkBootstrap.h"

#include "VkDraw.h"
#include "VkInit.h"

#include "glm/glm.hpp"

#define VMA_IMPLEMENTATION
#include "ImGuiData.h"
#include "Mesh.h"
#include "memory"
#include "vma/vk_mem_alloc.h"

// TODO: VkCommandPool and VkCommands creation manager;

const std::string ENGINE_NAME = "Silent Engine";
const uint32_t WIDTH = 1920;
const uint32_t HEIGHT = 1080;

uint64_t _currentFrame = 0;
double _currentTime { 0.0 };

vkb::Instance _instance;
vkb::PhysicalDevice _physicalDevice;
vkb::Device _device;
vkb::Swapchain _swapchain;

VkSurfaceKHR _surface;

std::vector<VkImage> _swapchainImages;
std::vector<VkImageView> _swapchainImageViews;
std::vector<VkFramebuffer> _swapchainFramebuffers;

VkCommandPool _commandPool;
VkRenderPass _renderPass;

VkDescriptorPool _descriptorPool;
VkDescriptorSetLayout _defaultDescriptorSetLayout;

VkPipelineLayout _defaultPipelineLayout;

VmaAllocator _allocator;

std::unique_ptr<ImGuiData> _imGuiData;

std::unique_ptr<Mesh> _mesh;

std::vector<Vertex> _vertices {
    { { 0.0f, -0.5f, 0.0f } },
    { { 0.5f, 0.5f, 0.0f } },
    { { -0.5f, 0.5f, 0.0f } },
};

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void init(GLFWwindow* window);
void initMesh();
void cleanup();
void draw();

int main()
{
    if (!glfwInit()) {
        throw std::runtime_error("Error: glfwInit");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Silent Engine", NULL, NULL);
    if (!window) {
        glfwTerminate();

        throw std::runtime_error("Error: glfwCreateWindow");
    }

    glfwSetKeyCallback(window, key_callback);

    init(window);
    initMesh();

    _currentTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        const double frameTime = (currentTime - _currentTime) * 1000.0;
        const double fps = 1000.0 / frameTime;
        _currentTime = currentTime;

        glfwPollEvents();

        _imGuiData->setFrameData(_currentFrame, currentTime, frameTime, fps);
        _imGuiData->render();

        draw();

        _currentFrame++;
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

    auto physicalDeviceResult = selector.set_surface(_surface).set_desired_version(1, 1).select();
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

    _renderPass = VkInit::createRenderPass(_device, _swapchain);

    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        _swapchainFramebuffers[i] = VkInit::createFramebuffer(_device, _renderPass, _swapchainImageViews[i], WIDTH, HEIGHT);
    }

    _commandPool = VkInit::createCommandPool(_device);

    _allocator = VkInit::createAllocator(_instance, _physicalDevice, _device, VK_API_VERSION_1_0, _swapchain.image_count);

    _descriptorPool = VkInit::createDescriptorPool(_device);

    _defaultDescriptorSetLayout = VkInit::createDefaultDescriptorSetLayout(_device);

    _defaultPipelineLayout = VkInit::Pipeline::createDefaultPipelineLayout(_device, 1, &_defaultDescriptorSetLayout);

    _imGuiData = std::make_unique<ImGuiData>(window, _instance.instance, _physicalDevice.physical_device, _device.device,
        _device.get_queue_index(vkb::QueueType::graphics).value(), _device.get_queue(vkb::QueueType::graphics).value(), _swapchain.image_count, _renderPass, _commandPool);
}

void initMesh()
{
    _mesh = std::make_unique<Mesh>(_device, _allocator, _defaultPipelineLayout, _renderPass, WIDTH, HEIGHT, _commandPool, _vertices);
}

void cleanup()
{
    _mesh.reset();
    _imGuiData.reset();

    vkDestroyPipelineLayout(_device.device, _defaultPipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(_device.device, _defaultDescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(_device.device, _descriptorPool, nullptr);

    vmaDestroyAllocator(_allocator);

    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        vkDestroyFramebuffer(_device.device, _swapchainFramebuffers[i], nullptr);
    }

    vkDestroyRenderPass(_device.device, _renderPass, nullptr);

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
        throw std::runtime_error("Error: vkAcquireNextImageKHR");
    }

    auto gray = [&]() {
        return abs(sinf(_currentFrame / 100.0f));
    }();

    VkClearValue clearValue { gray, gray, gray };

    VkCommandBuffer cmd = VkDraw::recordCommandBuffer(_device, _commandPool, _mesh.get(), _renderPass, _swapchainFramebuffers[imageIndex],
        VkRect2D { 0, 0, WIDTH, HEIGHT }, 1, &clearValue, _imGuiData.get());    

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

    vkFreeCommandBuffers(_device.device, _commandPool, 1, &cmd);
    vkDestroyFence(_device.device, queueFence, nullptr);
    vkDestroySemaphore(_device.device, acquireSemaphore, nullptr);
    vkDestroySemaphore(_device.device, submitSemaphore, nullptr);
}