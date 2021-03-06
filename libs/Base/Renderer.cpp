#include "Renderer.h"

#include <memory>
#include <stdexcept>

#include "VkDraw.h"
#include "VkInit.h"

#include "vk_mem_alloc.h"
#include <limits>

Renderer::Renderer(const std::shared_ptr<Window> &window) {
    _window = window;
    vkb::InstanceBuilder builder;
    auto instanceResult = builder.set_app_name(_window->getName().c_str()).request_validation_layers(
            true).use_default_debug_messenger().require_api_version(1, 2, 0).build();

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

    vkb::DeviceBuilder deviceBuilder{_physicalDevice};

    auto deviceResult = deviceBuilder.build();
    if (!deviceResult) {
        throw std::runtime_error(deviceResult.error().message());
    }

    _device = deviceResult.value();

    vkb::SwapchainBuilder swapchainBuilder{_device};

    auto swapchainResult = swapchainBuilder.use_default_format_selection().set_desired_present_mode(
            VK_PRESENT_MODE_IMMEDIATE_KHR).set_desired_extent(_window->getWidth(), _window->getHeight()).build();
    if (!swapchainResult) {
        throw std::runtime_error(swapchainResult.error().message());
    }

    _allocator = VkInit::createAllocator(_instance, _physicalDevice, _device, VK_API_VERSION_1_2);

    _swapchain = swapchainResult.value();
    _swapchainImages = _swapchain.get_images().value();
    _swapchainImageViews = _swapchain.get_image_views().value();

    //

    _renderPass = VkInit::createRenderPass(_device, _swapchain);

    _depthStencilImages = std::vector<std::unique_ptr<Image>>(_swapchain.image_count);
    _framebuffers = std::vector<VkFramebuffer>(_swapchain.image_count);
    _frameDatas = std::vector<FrameData>(_swapchain.image_count);
    _frameCommandPools = std::vector<VkCommandPool>(_swapchain.image_count);
    for (uint32_t i = 0; i < _swapchain.image_count; ++i) {
        _depthStencilImages[i] = std::make_unique<Image>(_device, _allocator, _window->getWidth(),
                                                         _window->getHeight());
        _framebuffers[i] = VkInit::createFramebuffer(_device, _renderPass, _swapchainImageViews[i],
                                                     _depthStencilImages[i]->getImageView(), _window->getWidth(),
                                                     _window->getHeight());
        _frameDatas[i] = FrameData(_device.device);
        _frameCommandPools[i] = VkInit::createCommandPool(_device);
    }

    _commandPool = VkInit::createCommandPool(_device);

    _imGuiData = ImGuiData(_window->getInternalWindow(), _instance.instance, _physicalDevice.physical_device,
                           _device.device,
                           _device.get_queue_index(vkb::QueueType::graphics).value(),
                           _device.get_queue(vkb::QueueType::graphics).value(), _swapchain.image_count, _renderPass,
                           _commandPool);

    _textureManager = TextureManager(_device, _allocator, _commandPool);
    _bufferManager = BufferManager(_device, _allocator, _commandPool);
    _imageManager = ImageManager(_device, _allocator, _commandPool);
    _samplerManager = SamplerManager(_device, _allocator, _commandPool);
    _descriptorSetLayoutManager = std::make_shared<DescriptorSetLayoutManager>(_device);
    _descriptorSetManager = std::make_shared<DescriptorSetManager>(_device, _descriptorSetLayoutManager);
    _pipelineLayoutManager = std::make_shared<PipelineLayoutManager>(_device, _descriptorSetLayoutManager);
    _pipelineManager = std::make_shared<PipelineManager>(_device, static_cast<float>(window->getWidth()),
                                                         static_cast<float>(window->getHeight()), _renderPass,
                                                         _pipelineLayoutManager);
    _materialManager = MaterialManager(_device, _pipelineManager, _descriptorSetManager);
}

Renderer::~Renderer() {
    for (auto &frameData: _frameDatas) {
        frameData.wait();
        frameData.reset();
    }

    _bufferManager.destroy();
    _imageManager.destroy();
    _samplerManager.destroy();
    _textureManager.destroy();
    _materialManager.destroy();
    _pipelineManager->destroy();
    _descriptorSetManager->destroy();
    _pipelineLayoutManager->destroy();
    _descriptorSetLayoutManager->destroy();

    for (auto &image: _depthStencilImages) {
        image->destroy(_device.device, _allocator);
    }
    _imGuiData.destroy(_device.device, _allocator);

    vmaDestroyAllocator(_allocator);

    for (auto &framebuffer: _framebuffers) {
        vkDestroyFramebuffer(_device.device, framebuffer, nullptr);
    }

    vkDestroyRenderPass(_device.device, _renderPass, nullptr);

    for(auto& commandPool : _frameCommandPools) {
        vkDestroyCommandPool(_device.device, commandPool, nullptr);
    }
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
                 std::function<void(const std::string &, const std::shared_ptr<Renderer> &,
                                    std::vector<std::shared_ptr<Entity>> &,
                                    std::vector<std::shared_ptr<MeshComponent>> &)> onFileSelected) {

    _currentAccumulatedTime += deltaTime;
    _currentAccumulatedFrames++;
    if (_currentAccumulatedTime > 1) {
        _currentAverageFPS = _currentAccumulatedFrames / _currentAccumulatedTime;

        _currentAccumulatedFrames = 0;
        _currentAccumulatedTime = 0.0f;
    }
    _imGuiData.setFrameData(_currentFrame, currentTime, deltaTime * 1000.0f, _currentAverageFPS);
    _imGuiData.setCameraPosition(drawData.getCamera()->getPosition());
    _imGuiData.render(drawEditor, [&](const std::string &filePath) {
        onFileSelected(filePath, renderer, entities, meshComponents);
    });

    draw(drawData);
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

std::shared_ptr<Buffer> Renderer::getBuffer(const std::string &name) {
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

void Renderer::draw(const DrawData &drawData) {
    auto acquireSemaphore = VkInit::createSemaphore(_device);
    auto submitSemaphore = VkInit::createSemaphore(_device);
    auto queueFence = VkInit::createFence(_device, {});
    uint32_t imageIndex;

    if (vkAcquireNextImageKHR(_device.device, _swapchain.swapchain, (std::numeric_limits<uint64_t>::max)() / 2,
                              acquireSemaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkAcquireNextImageKHR");
    }

    FrameData &_frameData = _frameDatas[imageIndex];
    _frameData.wait();
    _frameData.reset();
    _frameData.addFence(queueFence);
    _frameData.addSemaphore(acquireSemaphore);
    _frameData.addSemaphore(submitSemaphore);

    VkQueue graphicsQueue = _device.get_queue(vkb::QueueType::graphics).value();

    VkCommandBuffer cmd = VkDraw::recordCommandBuffer(_device, _frameCommandPools[imageIndex], drawData, _renderPass,
                                                      _framebuffers[imageIndex], VkRect2D{{0,                   0},
                                                                                          {_window->getWidth(), _window->getHeight()}},
                                                      _imGuiData);

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

    _frameData.addCommand(_frameCommandPools[imageIndex], cmd);
}