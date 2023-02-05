#include "Renderer.h"

#include <memory>
#include <stdexcept>

#include "VkDraw.h"
#include "VkInit.h"

#include "vk_mem_alloc.h"
#include <limits>
#include <iostream>

Renderer::Renderer(const std::shared_ptr<Window> window)
        : _window(std::move(window)) {

    _renderArea = VkRect2D{{0,                   0},
                           {_window->getWidth(), _window->getHeight()}};

    vkb::InstanceBuilder builder;
    auto instanceResult = builder.set_app_name(_window->getName().c_str()).request_validation_layers(
            true).use_default_debug_messenger().require_api_version(1, 3, 0).build();

    if (!instanceResult) {
        throw std::runtime_error(instanceResult.error().message());
    }

    _instance = instanceResult.value();

    _surface = _window->createSurface(_instance.instance);

    vkb::PhysicalDeviceSelector selector{_instance};

    auto physicalDeviceResult = selector.set_surface(_surface).set_minimum_version(1, 2).select();
    if (!physicalDeviceResult) {
        throw std::runtime_error(physicalDeviceResult.error().message());
    }

    _physicalDevice = physicalDeviceResult.value();

    VkPhysicalDeviceVulkan13Features vulkan13Features{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .pNext = nullptr,
            .robustImageAccess = VK_FALSE,
            .inlineUniformBlock = VK_FALSE,
            .descriptorBindingInlineUniformBlockUpdateAfterBind = VK_FALSE,
            .pipelineCreationCacheControl = VK_FALSE,
            .privateData = VK_FALSE,
            .shaderDemoteToHelperInvocation = VK_FALSE,
            .shaderTerminateInvocation = VK_FALSE,
            .subgroupSizeControl = VK_FALSE,
            .computeFullSubgroups = VK_FALSE,
            .synchronization2 = VK_FALSE,
            .textureCompressionASTC_HDR = VK_FALSE,
            .shaderZeroInitializeWorkgroupMemory = VK_FALSE,
            .dynamicRendering = VK_TRUE,
            .shaderIntegerDotProduct = VK_FALSE,
            .maintenance4 = VK_FALSE,
    };

    vkb::DeviceBuilder deviceBuilder{_physicalDevice};

    auto deviceResult = deviceBuilder.add_pNext(&vulkan13Features).build();
    if (!deviceResult) {
        throw std::runtime_error(deviceResult.error().message());
    }

    _device = deviceResult.value();

    vkb::SwapchainBuilder swapchainBuilder{_device};

    auto swapchainResult = swapchainBuilder.use_default_format_selection().set_desired_present_mode(
            VK_PRESENT_MODE_IMMEDIATE_KHR).set_desired_format(
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}).set_desired_extent(_window->getWidth(),
                                                                                              _window->getHeight()).build();
    if (!swapchainResult) {
        throw std::runtime_error(swapchainResult.error().message());
    }

    _allocator = VkInit::createAllocator(_instance, _physicalDevice, _device, VK_API_VERSION_1_2);

    _swapchain = swapchainResult.value();

    std::vector<VkImage> swapchainImages = _swapchain.get_images().value();
    _swapchainImageViews = _swapchain.get_image_views().value();

    for (size_t i = 0; i < _swapchain.image_count; ++i) {
        _frameResource.emplace_back(
                _device.device, _allocator, swapchainImages[i], _swapchainImageViews[i], _renderArea);
    }

    _commandPool = VkInit::createCommandPool(_device);

    _textureManager = TextureManager(_device, _allocator, _commandPool);
    _bufferManager = BufferManager(_device, _allocator, _commandPool);
    _imageManager = ImageManager(_device, _allocator, _commandPool);
    _samplerManager = SamplerManager(_device, _allocator, _commandPool);
    _descriptorSetLayoutManager = std::make_shared<DescriptorSetLayoutManager>(_device);
    _descriptorSetManager = std::make_shared<DescriptorSetManager>(_device, _descriptorSetLayoutManager);
    _pipelineLayoutManager = std::make_shared<PipelineLayoutManager>(_device, _descriptorSetLayoutManager);
    _pipelineManager = std::make_shared<PipelineManager>(_device, static_cast<float>(window->getWidth()),
                                                         static_cast<float>(window->getHeight()),
                                                         _pipelineLayoutManager);
    _materialManager = MaterialManager(_device, _pipelineManager, _descriptorSetManager);
}

Renderer::~Renderer() {
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
        frameResources.destroy(_device, _allocator);
    }

    vmaDestroyAllocator(_allocator);

    vkDestroyCommandPool(_device.device, _commandPool, nullptr);

    _swapchain.destroy_image_views(_swapchainImageViews);
    vkb::destroy_swapchain(_swapchain);

    _window->destroySurface(_instance.instance, _surface);

    vkb::destroy_device(_device);
    vkb::destroy_instance(_instance);
}

void
Renderer::update(const DrawData &drawData, float currentTime, float deltaTime, bool drawEditor,
                 const std::shared_ptr<Renderer> &renderer, std::vector<std::shared_ptr<Entity>> &entities,
                 std::vector<std::shared_ptr<MeshComponent>> &meshComponents,
                 std::function<void(const std::shared_ptr<Renderer> &,
                                    std::vector<std::shared_ptr<Entity>> &,
                                    std::vector<std::shared_ptr<MeshComponent>> &)> onFileSelected) {

    _currentAccumulatedTime += deltaTime;
    _currentAccumulatedFrames++;
    if (_currentAccumulatedTime > 1) {
        _currentAverageFPS = _currentAccumulatedFrames / _currentAccumulatedTime;

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
    _textureManager.addTexture(name, sampler, image);
}

std::shared_ptr<Texture> Renderer::getTexture(const std::string &name) {
    return _textureManager.getTexture(name);
}

std::shared_ptr<Material> Renderer::getMaterial(const std::vector<VertexAttributeDescription> &descriptions,
                                                const std::vector<VkDescriptorType> &types,
                                                std::vector<std::shared_ptr<Texture>> &textures) {
    return _materialManager.getMaterial(descriptions, types, textures);
}

void Renderer::draw(const DrawData &drawData, const VkRect2D renderArea) {
    auto acquireSemaphore = VkInit::createSemaphore(_device);
    auto submitSemaphore = VkInit::createSemaphore(_device);
    auto queueFence = VkInit::createFence(_device, {});

    uint32_t imageIndex;

    VkResult result = VK_TIMEOUT;
    do {
        result = vkAcquireNextImageKHR(_device.device, _swapchain.swapchain, (std::numeric_limits<uint64_t>::max)(),
                                       acquireSemaphore, VK_NULL_HANDLE, &imageIndex);
    } while (result != VK_SUCCESS);

    VkQueue graphicsQueue = _device.get_queue(vkb::QueueType::graphics).value();

    FrameResources &frameResources = _frameResource[imageIndex];

    VkCommandBuffer cmd = VkDraw::recordCommandBuffer(_device, _commandPool, drawData,
                                                      frameResources.getSwapchainImage(),
                                                      frameResources.getSwapchainImageView(),
                                                      _device.get_queue_index(vkb::QueueType::graphics).value(),
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

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, queueFence);

    VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &submitSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &_swapchain.swapchain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr,
    };

    vkQueuePresentKHR(graphicsQueue, &presentInfo);

    vkWaitForFences(_device.device, 1, &queueFence, VK_TRUE, (std::numeric_limits<uint64_t>::max)());

    vkDestroySemaphore(_device.device, acquireSemaphore, nullptr);
    vkDestroySemaphore(_device.device, submitSemaphore, nullptr);
    vkDestroyFence(_device.device, queueFence, nullptr);
}