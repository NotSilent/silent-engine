#include "Renderer.h"

#include <memory>
#include <stdexcept>

#include "VkInit.h"

#include "vk_mem_alloc.h"
#include <limits>
#include <iostream>
#include <utility>
#include "Image.h"

Renderer::Renderer(const std::shared_ptr<Window> &window)
        : window(window), instance(createInstance()), surface(createSurface()), physicalDevice(selectPhysicalDevice()),
          device(createDevice()), swapchain(createSwapchain()),
          _renderArea(VkRect2D{{0, 0}, {window->getWidth(), window->getHeight()}}),
          swapchainImageFormat(vk::Format(swapchain.image_format)),
          _pipelineManager(device.device, swapchainImageFormat, _renderArea) {
    _allocator = VkInit::createAllocator(instance.instance, physicalDevice.physical_device, device.device, VK_API_VERSION_1_3);

    uint32_t queueFamilyIndex = device.get_queue_index(vkb::QueueType::graphics).value();;

    _commandPool = VkInit::createCommandPool(device.device, device.get_queue_index(vkb::QueueType::graphics).value());

    _bufferManager = BufferManager(device, _allocator, _commandPool);
    _imageManager = ImageManager(device.device, graphicsQueue.queue, _allocator, _commandPool);

    std::vector<VkImage> swapchainImages = swapchain.get_images().value();
    _swapchainImageViews = swapchain.get_image_views().value();

    for (size_t i = 0; i < swapchain.image_count; ++i) {
        _frameResource.emplace_back(
                device.device, _allocator, queueFamilyIndex, Image(swapchainImages[i], _swapchainImageViews[i]), _pipelineManager, _renderArea);
    }

    presentFence = VkInit::createFence(device.device, {});

    graphicsQueue.queue = device.get_queue(vkb::QueueType::graphics).value();
    graphicsQueue.familyIndex = device.get_queue_index(vkb::QueueType::graphics).value();
}

vkb::Instance Renderer::createInstance() {
    vkb::InstanceBuilder builder;
    auto instanceResult = builder.set_app_name(window->getName().c_str()).request_validation_layers(
            true).use_default_debug_messenger().require_api_version(1, 3, 0).build();

    if (!instanceResult) {
        throw std::runtime_error(instanceResult.error().message());
    }

    return instanceResult.value();
}

VkSurfaceKHR Renderer::createSurface() {
    return window->createSurface(instance);
}

vkb::PhysicalDevice Renderer::selectPhysicalDevice() {
    vkb::PhysicalDeviceSelector selector{instance};

    auto physicalDeviceResult = selector.set_surface(surface).set_minimum_version(1, 3).select();
    if (!physicalDeviceResult) {
        throw std::runtime_error(physicalDeviceResult.error().message());
    }

    return physicalDeviceResult.value();
}

vkb::Device Renderer::createDevice() {
    VkPhysicalDeviceVulkan13Features vulkan13Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .synchronization2 = true,
            .dynamicRendering = true,
    };

    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    auto deviceResult = deviceBuilder.add_pNext(&vulkan13Features).build();
    if (!deviceResult) {
        throw std::runtime_error(deviceResult.error().message());
    }

    return deviceResult.value();
}

vkb::Swapchain Renderer::createSwapchain() {
    vkb::SwapchainBuilder swapchainBuilder{device};

    auto swapchainResult = swapchainBuilder
            .use_default_format_selection()
            .set_desired_present_mode(VK_PRESENT_MODE_IMMEDIATE_KHR)
            .set_desired_format({VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
            .set_desired_extent(window->getWidth(), window->getHeight()).build();

    if (!swapchainResult) {
        throw std::runtime_error(swapchainResult.error().message());
    }

    return swapchainResult.value();
}

Renderer::~Renderer() {
    for (FrameResources &frameResources: _frameResource) {
        frameResources.destroy();
    }

    vkDestroyFence(device, presentFence, nullptr);

    _bufferManager.destroy();
    _imageManager.destroy();
    _pipelineManager.destroy();

    vmaDestroyAllocator(_allocator);

    vkDestroyCommandPool(device.device, _commandPool, nullptr);

    swapchain.destroy_image_views(_swapchainImageViews);
    vkb::destroy_swapchain(swapchain);

    window->destroySurface(instance.instance, surface);

    vkb::destroy_device(device);
    vkb::destroy_instance(instance);
}

void
Renderer::update(const DrawData &drawData, float currentTime, float deltaTime) {
    _currentFrame++;
    _currentAccumulatedTime += deltaTime;
    _currentAccumulatedFrames++;
    if (_currentAccumulatedTime > 1) {
        _currentAverageFPS = static_cast<float>(_currentAccumulatedFrames) / _currentAccumulatedTime;

        _currentAccumulatedFrames = 0;
        _currentAccumulatedTime = 0.0f;
        std::cout << "Average fps: " << _currentAverageFPS << '\n';
    }

    draw(drawData, _renderArea);
}

void Renderer::addBuffer(const std::string &name, VkBufferUsageFlags flags, uint32_t size, const void *data) {
    _bufferManager.addBuffer(name, flags, size, data);
}

VkBuffer Renderer::getBuffer(const std::string &name) {
    return _bufferManager.getBuffer(name);
}

void Renderer::addImage(const std::string &name, uint32_t width, uint32_t height, uint32_t size, const void *data) {
    _imageManager.addImage(name, width, height, size, data);
}

std::shared_ptr<Image> Renderer::getImage(const std::string &name) {
    return _imageManager.getImage(name);
}

void Renderer::draw(const DrawData &drawData, const VkRect2D renderArea) {
    VkSemaphore acquireSemaphore = VkInit::createSemaphore(device.device);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device.device, swapchain.swapchain, std::numeric_limits<uint64_t>::max(),
                          acquireSemaphore, presentFence, &imageIndex);

    FrameResources &frameResources = _frameResource[imageIndex];
    frameResources.renderFrame(swapchain.swapchain, graphicsQueue.queue, imageIndex, acquireSemaphore, presentFence, drawData);
}

VkPipelineLayout Renderer::getDeferredPipelineLayout() const {
    return _pipelineManager.getDeferredPipelineLayout();
}

VkPipeline Renderer::getDeferredPipeline() const {
    return _pipelineManager.getDeferredPipeline();
}