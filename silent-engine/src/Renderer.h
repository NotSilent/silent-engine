#pragma once
#include "vk-bootstrap/VkBootstrap.h"

#include "ImGuiData.h"
#include "Mesh.h"

#include "EngineManagers/BufferManager.h"
#include "Camera.h"
#include "EngineManagers/DescriptorSetLayoutManager.h"
#include "EngineManagers/DescriptorSetManager.h"
#include "DrawData.h"
#include "Image.h"
#include "EngineManagers/ImageManager.h"
#include "EngineManagers/PipelineLayoutManager.h"
#include "EngineManagers/PipelineManager.h"
#include "EngineManagers/SamplerManager.h"
#include "Texture.h"
#include "EngineManagers/TextureManager.h"
#include "EngineManagers/MaterialManager.h"
#include <Window.h>

// TODO: VkCommandPool and VkCommands creation manager;

class Renderer {
public:
    Renderer(std::shared_ptr<Window> window);
    ~Renderer();

    // TODO: Reduce input to DrawData and EditorDrawData
    void update(const DrawData& drawData, float currentTime, float deltaTime, uint64_t currentFrame);

    void addSampler(const std::string& name);
    std::shared_ptr<Sampler> getSampler(const std::string& name);

    void addBuffer(const std::string& name, uint32_t size, const void* data);
    std::shared_ptr<Buffer> getBuffer(const std::string& name);

    void addImage(const std::string& name, uint32_t width, uint32_t height, uint32_t size, const void* data);
    std::shared_ptr<Image> getImage(const std::string& name);

    void addTexture(const std::string& name, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image);
    std::shared_ptr<Texture> getTexture(const std::string& name);

    std::shared_ptr<Material> getMaterial(const std::vector<VertexAttributeDescription>& descriptions, const std::vector<VkDescriptorType>& types, std::vector<std::shared_ptr<Texture>>& textures);

private:
    void draw(const DrawData& drawData);

    std::shared_ptr<Window> _window;

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

    VmaAllocator _allocator;

    ImGuiData _imGuiData;

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
