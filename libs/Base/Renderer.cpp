#include "Renderer.h"

#include <memory>
#include <stdexcept>

#include "VkDraw.h"
#include "VkInit.h"

#include "vk_mem_alloc.h"
#include <limits>
#include <iostream>
#include <utility>

Renderer::Renderer(const std::shared_ptr<Window> &window)
        : window(window), instance(createInstance()), surface(createSurface()), physicalDevice(selectPhysicalDevice()),
          device(createDevice()), swapchain(createSwapchain()),
          _renderArea(VkRect2D{{0,                  0},
                               {window->getWidth(), window->getHeight()}}) {
    _allocator = VkInit::createAllocator(instance, physicalDevice, device, VK_API_VERSION_1_3);

    std::vector<VkImage> swapchainImages = swapchain.get_images().value();
    _swapchainImageViews = swapchain.get_image_views().value();

    uint32_t queueFamilyIndex = device.get_queue_index(vkb::QueueType::graphics).value();;

    for (size_t i = 0; i < swapchain.image_count; ++i) {
        _frameResource.emplace_back(
                device.device, _allocator, queueFamilyIndex, swapchainImages[i], _swapchainImageViews[i], _renderArea);
    }

    _commandPool = VkInit::createCommandPool(device, device.get_queue_index(vkb::QueueType::graphics).value());

    _textureManager = TextureManager(device, _allocator, _commandPool);
    _bufferManager = BufferManager(device, _allocator, _commandPool);
    _imageManager = ImageManager(device, _allocator, _commandPool);
    _samplerManager = SamplerManager(device, _allocator, _commandPool);
    _pipelineLayoutManager = std::make_shared<PipelineLayoutManager>(device);
    _pipelineManager = std::make_shared<PipelineManager>(device, static_cast<float>(window->getWidth()),
                                                         static_cast<float>(window->getHeight()),
                                                         _pipelineLayoutManager);
    _materialManager = MaterialManager(device, _pipelineManager);

    presentFence = VkInit::createFence(device, {});
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
    _samplerManager.destroy();
    _textureManager.destroy();
    _materialManager.destroy();
    _pipelineManager->destroy();
    _pipelineLayoutManager->destroy();

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

void Renderer::addSampler(const std::string &name) {
    _samplerManager.addSampler(name);
}

std::shared_ptr<Sampler> Renderer::getSampler(const std::string &name) {
    return _samplerManager.getSampler(name);
}

void Renderer::addBuffer(const std::string &name, uint32_t size, const void *data) {
    _bufferManager.addBuffer(name, size, data);
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

void Renderer::addTexture(const std::string &name, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image) {
    _textureManager.addTexture(name, std::move(sampler), std::move(image));
}

std::shared_ptr<Texture> Renderer::getTexture(const std::string &name) {
    return _textureManager.getTexture(name);
}

std::optional<std::shared_ptr<Material>>
Renderer::getMaterial(const std::vector<VertexAttributeDescription> &descriptions,
                      const std::vector<VkDescriptorType> &types,
                      std::vector<std::shared_ptr<Texture>> &textures) {
    return _materialManager.getMaterial(descriptions, types, textures);
}

void Renderer::draw(const DrawData &drawData, const VkRect2D renderArea) {
    VkSemaphore acquireSemaphore = VkInit::createSemaphore(device);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device.device, swapchain.swapchain, std::numeric_limits<uint64_t>::max(),
                          acquireSemaphore, presentFence, &imageIndex);

    VkQueue graphicsQueue = device.get_queue(vkb::QueueType::graphics).value();

    FrameResources &frameResources = _frameResource[imageIndex];
    frameResources.prepareNewFrame(swapchain.swapchain, graphicsQueue, imageIndex, acquireSemaphore, drawData, renderArea);

    vkWaitForFences(device, 1, &presentFence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(device, 1, &presentFence);
}
