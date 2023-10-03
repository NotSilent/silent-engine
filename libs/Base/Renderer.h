#pragma once

#include "VkBootstrap.h"

#include "Mesh.h"

#include "BufferManager.h"
#include "Camera.h"
#include "DrawData.h"
#include "ImageManager.h"
#include "PipelineManager.h"
#include "SamplerManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Window.h"
#include "Sampler.h"
#include "Buffer.h"
#include "Image.h"
#include "Entity.h"
#include "MeshComponent.h"
#include "FrameResources.h"
#include "Queue.h"
#include <functional>

// TODO: VkCommandPool and VkCommands creation manager;

class Renderer {
public:
    explicit Renderer(const std::shared_ptr<Window>& window);

    ~Renderer();

    void update(const DrawData &drawData, float currentTime, float deltaTime);

    void addSampler(const std::string &name);

    std::shared_ptr<Sampler> getSampler(const std::string &name);

    void addBuffer(const std::string &name, uint32_t size, const void *data);

    VkBuffer getBuffer(const std::string &name);

    void addImage(const std::string &name, uint32_t width, uint32_t height, uint32_t size, const void *data);

    std::shared_ptr<Image> getImage(const std::string &name);

    void addTexture(const std::string &name, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image);

    std::shared_ptr<Texture> getTexture(const std::string &name);


    // TODO: Technicaly unsafe
    [[nodiscard]] VkPipelineLayout getDeferredPipelineLayout() const;

    [[nodiscard]] VkPipeline getDeferredPipeline() const;

    [[nodiscard]] VkPipeline getCompositePipeline() const;

private:
    void draw(const DrawData &drawData, VkRect2D renderArea);

    std::shared_ptr<Window> window;

    VkRect2D _renderArea;

    uint64_t _currentFrame = 0;

    float _currentAverageFPS = 0.0f;
    float _currentAccumulatedTime = 0.0f;
    uint64_t _currentAccumulatedFrames = 0;

    // Remove vkb?
    vkb::Instance instance;
    VkSurfaceKHR surface;
    vkb::PhysicalDevice physicalDevice;
    vkb::Device device;
    vkb::Swapchain swapchain;


    vkb::Instance createInstance();
    VkSurfaceKHR createSurface();
    vkb::PhysicalDevice selectPhysicalDevice();
    vkb::Device createDevice();
    vkb::Swapchain createSwapchain();

    VkCommandPool _commandPool;

    VmaAllocator _allocator;

    VkFence presentFence;

    Queue graphicsQueue;

    // TODO: Remove?
    std::vector<VkImageView> _swapchainImageViews;
    std::vector<FrameResources> _frameResource;

    BufferManager _bufferManager;
    ImageManager _imageManager;
    TextureManager _textureManager;
    SamplerManager _samplerManager;
    std::shared_ptr<PipelineManager> _pipelineManager;
};
