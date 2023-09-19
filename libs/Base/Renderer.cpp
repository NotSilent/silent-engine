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
          device(createDevice()), swapchain(createSwapchain()), synchronizationManager(device.device),
          _renderArea(VkRect2D{{0,                  0},
                               {window->getWidth(), window->getHeight()}}) {
    _allocator = VkInit::createAllocator(instance, physicalDevice, device, VK_API_VERSION_1_3);

    std::vector<VkImage> swapchainImages = swapchain.get_images().value();
    _swapchainImageViews = swapchain.get_image_views().value();

    for (size_t i = 0; i < swapchain.image_count; ++i) {
        _frameResource.emplace_back(
                device.device, _allocator, swapchainImages[i], _swapchainImageViews[i], _renderArea);
    }

    _commandPool = VkInit::createCommandPool(device, device.get_queue_index(vkb::QueueType::graphics).value());

    _textureManager = TextureManager(device, _allocator, _commandPool);
    _bufferManager = BufferManager(device, _allocator, _commandPool);
    _imageManager = ImageManager(device, _allocator, _commandPool);
    _samplerManager = SamplerManager(device, _allocator, _commandPool);
    _descriptorSetLayoutManager = std::make_shared<DescriptorSetLayoutManager>(device);
    _descriptorSetManager = std::make_shared<DescriptorSetManager>(device, _descriptorSetLayoutManager);
    _pipelineLayoutManager = std::make_shared<PipelineLayoutManager>(device, _descriptorSetLayoutManager);
    _pipelineManager = std::make_shared<PipelineManager>(device, static_cast<float>(window->getWidth()),
                                                         static_cast<float>(window->getHeight()),
                                                         _pipelineLayoutManager);
    _materialManager = MaterialManager(device, _pipelineManager, _descriptorSetManager);
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
            .pNext = nullptr,
            .robustImageAccess = false,
            .inlineUniformBlock = false,
            .descriptorBindingInlineUniformBlockUpdateAfterBind = false,
            .pipelineCreationCacheControl = false,
            .privateData = false,
            .shaderDemoteToHelperInvocation = false,
            .shaderTerminateInvocation = false,
            .subgroupSizeControl = false,
            .computeFullSubgroups = false,
            .synchronization2 = false,
            .textureCompressionASTC_HDR = false,
            .shaderZeroInitializeWorkgroupMemory = false,
            .dynamicRendering = true,
            .shaderIntegerDotProduct = false,
            .maintenance4 = false,
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
    synchronizationManager.destroy();
    _bufferManager.destroy();
    _imageManager.destroy();
    _samplerManager.destroy();
    _textureManager.destroy();
    _materialManager.destroy();
    _pipelineManager->destroy();
    _descriptorSetManager->destroy();
    _pipelineLayoutManager->destroy();
    _descriptorSetLayoutManager->destroy();

    for (FrameResources &frameResources: _frameResource) {
        frameResources.destroy(device, _allocator);
    }

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
    auto acquireSemaphore = VkInit::createSemaphore(device);
    auto submitSemaphore = VkInit::createSemaphore(device);
    VkFence queueFence = VkInit::createFence(device, {});

    //VkFence presentationFinishedFence = VkInit::createFence(device, {});

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device.device, swapchain.swapchain, std::numeric_limits<uint64_t>::max(),
                          acquireSemaphore, nullptr /*presentationFinishedFence*/, &imageIndex);

    VkQueue graphicsQueue = device.get_queue(vkb::QueueType::graphics).value();

    FrameResources &frameResources = _frameResource[imageIndex];

    VkCommandBuffer cmd = VkDraw::recordCommandBuffer(device, _commandPool, drawData,
                                                      frameResources.getSwapchainImage(),
                                                      frameResources.getSwapchainImageView(),
                                                      device.get_queue_index(vkb::QueueType::graphics).value(),
                                                      renderArea);

    VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{
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

    //frameResources.waitFence();

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, queueFence);

    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &submitSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &swapchain.swapchain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr,
    };

    vkQueuePresentKHR(graphicsQueue, &presentInfo);

    vkWaitForFences(device, 1, &queueFence, true, std::numeric_limits<uint64_t>::max());
    vkDestroyFence(device, queueFence, nullptr);

    // TODO: build frame? wait for both queue and presentation? record time? reset command pool after wait for present

//    synchronizationManager.enqueue([presentationFinishedFence, acquireSemaphore, submitSemaphore](VkDevice newdevice) {
//        vkWaitForFences(newdevice, 1, &presentationFinishedFence, true, std::numeric_limits<uint64_t>::max());
//
//        vkDestroyFence(newdevice, presentationFinishedFence, nullptr);
//
//        vkDestroySemaphore(newdevice, acquireSemaphore, nullptr);
//        vkDestroySemaphore(newdevice, submitSemaphore, nullptr);
//    });
}
