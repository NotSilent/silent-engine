#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

#include "vk-bootstrap/VkBootstrap.h"

#include "VkDraw.h"
#include "VkInit.h"
#include "VkInitPipeline.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>

#include "ImGuiData.h"
#include "Mesh.h"
#include "memory"
#include "vma/vk_mem_alloc.h"

#include "Camera.h"
#include "Image.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "Texture.h"

// TODO: VkCommandPool and VkCommands creation manager;

const std::string ENGINE_NAME = "Silent Engine";
const std::string STANFORD_BUNNY_ASSET_LOCATION = "assets/stanford-bunny.obj";

const std::string PINK_TEXTURE_ASSET_LOCATION = "assets/pink.png";
const std::string TEST_TEXTURE_ASSET_LOCATION = "assets/test.png";

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

std::vector<std::unique_ptr<Image>> _depthStencilImages;

std::vector<VkFramebuffer> _framebuffers;

VkCommandPool _commandPool;
VkRenderPass _renderPass;

VkDescriptorPool _descriptorPool;
VkDescriptorSetLayout _defaultDescriptorSetLayout;
VkDescriptorSet _defaultDescriptorSet;

VkPipelineLayout _pipelineLayout;
VkPipeline _pipeline;

VmaAllocator _allocator;

ImGuiData _imGuiData;

MeshManager _meshManager;
TextureManager _textureManger;

Camera _camera;

// TODO: Input manager
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        _imGuiData.toggleShow();
    }
}

void init(GLFWwindow* window);
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    glfwSetCursorPos(window, 0.0, 0.0);

    glfwSetKeyCallback(window, key_callback);

    init(window);

    _currentTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        // deltaTime in seconds
        const double deltaTime = currentTime - _currentTime;
        // frameTIme in milliseconds
        const double frameTime = deltaTime * 1000.0;
        const double fps = 1000.0 / frameTime;
        _currentTime = currentTime;

        glfwPollEvents();

        glm::vec2 input { 0.0f };

        // TODO: Input manager
        // TODO: Add deltaTime
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            input.y += 1.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            input.y += -1.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            input.x += -1.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            input.x += 1.0f;
        }

        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);
        glfwSetCursorPos(window, 0.0, 0.0);

        _camera.update(deltaTime, input, glm::vec2(xPos, yPos));

        _imGuiData.setFrameData(_currentFrame, currentTime, frameTime, fps);
        _imGuiData.setCameraPosition(_camera.getPosition());
        _imGuiData.render();

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

    _allocator = VkInit::createAllocator(_instance, _physicalDevice, _device, VK_API_VERSION_1_1, _swapchain.image_count);

    _swapchain = swapchainResult.value();
    _swapchainImages = _swapchain.get_images().value();
    _swapchainImageViews = _swapchain.get_image_views().value();

    //

    _renderPass = VkInit::createRenderPass(_device, _swapchain);

    _depthStencilImages = std::vector<std::unique_ptr<Image>>(_swapchain.image_count);
    _framebuffers = std::vector<VkFramebuffer>(_swapchain.image_count);
    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        _depthStencilImages[i] = std::make_unique<Image>(_device, _allocator, WIDTH, HEIGHT);
        _framebuffers[i] = VkInit::createFramebuffer(_device, _renderPass, _swapchainImageViews[i], _depthStencilImages[i]->getImageView(), WIDTH, HEIGHT);
    }

    _commandPool = VkInit::createCommandPool(_device);

    _descriptorPool = VkInit::createDescriptorPool(_device);
    _defaultDescriptorSetLayout = VkInit::createDefaultDescriptorSetLayout(_device);
    _defaultDescriptorSet = VkInit::createDescriptorSet(_device, _descriptorPool, _defaultDescriptorSetLayout);

    _pipelineLayout = VkInit::Pipeline::createPipelineLayout(_device, 1, &_defaultDescriptorSetLayout, sizeof(PushData));
    _pipeline = VkInit::Pipeline::createDefaultPipeline(_device, _pipelineLayout, _renderPass, WIDTH, HEIGHT);

    _imGuiData = ImGuiData(window, _instance.instance, _physicalDevice.physical_device, _device.device,
        _device.get_queue_index(vkb::QueueType::graphics).value(), _device.get_queue(vkb::QueueType::graphics).value(), _swapchain.image_count, _renderPass, _commandPool);

    _textureManger = TextureManager(_device.device, _allocator, _commandPool, _device.get_queue(vkb::QueueType::graphics).value());
    _textureManger.addTexture(TEST_TEXTURE_ASSET_LOCATION);

    _meshManager = MeshManager(_device, _allocator, _commandPool);
    _meshManager.addMesh(STANFORD_BUNNY_ASSET_LOCATION, _textureManger.getTexture(TEST_TEXTURE_ASSET_LOCATION));

    _camera = Camera(WIDTH, HEIGHT);
}

void cleanup()
{
    _meshManager.destroy();
    _textureManger.destroy();

    for (auto& image : _depthStencilImages) {
        image->destroy(_device.device, _allocator);
    }
    _imGuiData.destroy(_device.device, _allocator);

    vkDestroyPipeline(_device.device, _pipeline, nullptr);
    vkDestroyPipelineLayout(_device.device, _pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(_device.device, _defaultDescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(_device.device, _descriptorPool, nullptr);

    vmaDestroyAllocator(_allocator);

    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        vkDestroyFramebuffer(_device.device, _framebuffers[i], nullptr);
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

    PushData pushData {
        .model = glm::mat4(1.0f),
        .view = _camera.getViewMatrix(),
        .projection = _camera.getProjectionMatrix(),
        .viewPosition = _camera.getPosition(),
    };

    const auto mesh = _meshManager.getMesh(STANFORD_BUNNY_ASSET_LOCATION);

    std::vector<std::shared_ptr<Mesh>> meshes;
    meshes.push_back(mesh);

    VkCommandBuffer cmd = VkDraw::recordCommandBuffer(_device.device, _commandPool, meshes, _pipelineLayout, _pipeline, _renderPass, _framebuffers[imageIndex],
        VkRect2D { 0, 0, WIDTH, HEIGHT }, _imGuiData, pushData, _defaultDescriptorSet);

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