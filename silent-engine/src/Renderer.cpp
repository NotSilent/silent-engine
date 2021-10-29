#include "Renderer.h"

#include <memory>
#include <stdexcept>

#include "VkDraw.h"
#include "VkInit.h"
#include "VkInitPipeline.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include "vma/vk_mem_alloc.h"
#include <glm/glm.hpp>

Renderer::Renderer(std::shared_ptr<Window> window)
{
    _window = window;
    vkb::InstanceBuilder builder;
    auto instanceResult = builder.set_app_name(_window->getName().c_str()).request_validation_layers(true).use_default_debug_messenger().build();

    if (!instanceResult) {
        throw std::runtime_error(instanceResult.error().message());
    }

    _instance = instanceResult.value();

    _surface = _window->createSurface(_instance.instance);

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

    auto swapchainResult = swapchainBuilder.use_default_format_selection().set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR).set_desired_extent(_window->getWidth(), _window->getHeight()).build();
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
        _depthStencilImages[i] = std::make_unique<Image>(_device, _allocator, _window->getWidth(), _window->getHeight());
        _framebuffers[i] = VkInit::createFramebuffer(_device, _renderPass, _swapchainImageViews[i], _depthStencilImages[i]->getImageView(), _window->getWidth(), _window->getHeight());
    }

    _commandPool = VkInit::createCommandPool(_device);

    _descriptorPool = VkInit::createDescriptorPool(_device);
    _defaultDescriptorSetLayout = VkInit::createDefaultDescriptorSetLayout(_device);
    _defaultDescriptorSet = VkInit::createDescriptorSet(_device, _descriptorPool, _defaultDescriptorSetLayout);

    _pipelineLayout = VkInit::Pipeline::createPipelineLayout(_device, 1, &_defaultDescriptorSetLayout, sizeof(PushData));
    _pipeline = VkInit::Pipeline::createDefaultPipeline(_device, _pipelineLayout, _renderPass, _window->getWidth(), _window->getHeight());

    _imGuiData = ImGuiData(_window->getInternalWindow(), _instance.instance, _physicalDevice.physical_device, _device.device,
        _device.get_queue_index(vkb::QueueType::graphics).value(), _device.get_queue(vkb::QueueType::graphics).value(), _swapchain.image_count, _renderPass, _commandPool);

    _textureManger = TextureManager(_device, _allocator, _commandPool);
    _meshManager = MeshManager(_device, _allocator, _commandPool);

    _currentTime = glfwGetTime();
}

Renderer::~Renderer()
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

    for (auto& framebuffer : _framebuffers) {
        vkDestroyFramebuffer(_device.device, framebuffer, nullptr);
    }

    vkDestroyRenderPass(_device.device, _renderPass, nullptr);

    vkDestroyCommandPool(_device.device, _commandPool, nullptr);

    _swapchain.destroy_image_views(_swapchainImageViews);
    vkb::destroy_swapchain(_swapchain);

    _window->destroySurface(_instance.instance, _surface);

    vkb::destroy_device(_device);
    vkb::destroy_instance(_instance);
}

void Renderer::update(const DrawData& drawData, float currentTime, float deltaTime, uint64_t currentFrame)
{
    float frameTime = deltaTime * 1000.0f;
    float fps = 1000.0f / frameTime;

    _imGuiData.setFrameData(_currentFrame, currentTime, frameTime, fps);
    _imGuiData.setCameraPosition(drawData.getCamera().getPosition());
    _imGuiData.render();

    draw(drawData);
}

std::shared_ptr<Mesh> Renderer::getMesh(const std::string& path)
{
    return _meshManager.getMesh(path);
}

std::shared_ptr<Texture> Renderer::getTexture(const std::string& path)
{
    return _textureManger.getTexture(path);
}

void Renderer::draw(const DrawData& drawData)
{
    auto acquireSemaphore = VkInit::createSemaphore(_device);
    uint32_t imageIndex;

    if (vkAcquireNextImageKHR(_device.device, _swapchain.swapchain, std::numeric_limits<uint32_t>::max(), acquireSemaphore, nullptr, &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkAcquireNextImageKHR");
    }

    VkCommandBuffer cmd = VkDraw::recordCommandBuffer(_device.device, _commandPool, drawData, _pipelineLayout, _pipeline, _renderPass, _framebuffers[imageIndex],
        VkRect2D { { 0, 0 }, { _window->getWidth(), _window->getHeight() } }, _imGuiData, _defaultDescriptorSet);

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