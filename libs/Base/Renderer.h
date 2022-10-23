#pragma once

#include "VkBootstrap.h"

//#include "ImGuiData.h"
#include "Mesh.h"

#include "BufferManager.h"
#include "Camera.h"
#include "DescriptorSetLayoutManager.h"
#include "DescriptorSetManager.h"
#include "DrawData.h"
#include "ImageManager.h"
#include "PipelineLayoutManager.h"
#include "PipelineManager.h"
#include "SamplerManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include "MaterialManager.h"
#include "Window.h"
#include "Sampler.h"
#include "Buffer.h"
#include "Image.h"
#include "Entity.h"
#include "MeshComponent.h"
#include "FrameData.h"
#include "DescriptorSet.h"
#include "RenderQueue.h"

// TODO: VkCommandPool and VkCommands creation manager;

class Renderer {
public:
    explicit Renderer(const std::shared_ptr<Window> &window);

    ~Renderer();

    // TODO: Reduce input to DrawData and EditorDrawData, separate editor
    void update(const DrawData &drawData, float currentTime, float deltaTime, bool drawEditor,
                const std::shared_ptr<Renderer> &renderer, std::vector<std::shared_ptr<Entity>> &entities,
                std::vector<std::shared_ptr<MeshComponent>> &meshComponents,
                std::function<void(const std::string &, const std::shared_ptr<Renderer> &,
                                   std::vector<std::shared_ptr<Entity>> &,
                                   std::vector<std::shared_ptr<MeshComponent>> &)> onFileSelected);

    void addSampler(const std::string &name);

    std::shared_ptr<Sampler> getSampler(const std::string &name);

    void addBuffer(const std::string &name, uint32_t size, const void *data);

    std::shared_ptr<Buffer> getBuffer(const std::string &name);

    void addImage(const std::string &name, uint32_t width, uint32_t height, uint32_t size, const void *data);

    std::shared_ptr<Image> getImage(const std::string &name);

    void addTexture(const std::string &name, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image);

    std::shared_ptr<Texture> getTexture(const std::string &name);

    std::shared_ptr<Material>
    getMaterial(const std::vector<VertexAttributeDescription> &descriptions, const std::vector<VkDescriptorType> &types,
                std::vector<std::shared_ptr<Texture>> &textures);

private:
    void draw(const DrawData &drawData);

    std::shared_ptr<Window> _window;

    uint64_t _currentFrame = 0;

    float _currentAverageFPS = 0.0f;
    float _currentAccumulatedTime = 0.0f;
    uint64_t _currentAccumulatedFrames = 0;

    vkb::Instance _instance;
    vkb::PhysicalDevice _physicalDevice;
    vkb::Device _device;
    vkb::Swapchain _swapchain;

    VkSurfaceKHR _surface;

    std::vector<VkImage> _swapchainImages;
    std::vector<VkImageView> _swapchainImageViews;

    std::vector<FrameData> _frameDatas;

    std::vector<VkCommandPool> _frameCommandPools;

    VkCommandPool _commandPool;

    VkRenderPass _renderPass;

    VmaAllocator _allocator;

    //ImGuiData _imGuiData;

    RenderQueue _renderQueue;

    BufferManager _bufferManager;
    ImageManager _imageManager;
    TextureManager _textureManager;
    SamplerManager _samplerManager;
    MaterialManager _materialManager;
    std::shared_ptr<PipelineManager> _pipelineManager;
    std::shared_ptr<DescriptorSetManager> _descriptorSetManager;
    std::shared_ptr<PipelineLayoutManager> _pipelineLayoutManager;
    std::shared_ptr<DescriptorSetLayoutManager> _descriptorSetLayoutManager;
};
