#pragma once
#include "vk-bootstrap/VkBootstrap.h"

#include "ImGuiData.h"
#include "Mesh.h"

#include "Camera.h"
#include "Image.h"
#include "MeshManager.h"
#include "BufferManager.h"
#include "Texture.h"
#include "TextureManager.h"
#include <Window.h>
#include "DrawData.h"

// TODO: VkCommandPool and VkCommands creation manager;

class Renderer {
public:
    Renderer(std::shared_ptr<Window> window);
    ~Renderer();

    // TODO: Reduce input to DrawData and EditorDrawData
    void update(const DrawData& drawData, float currentTime, float deltaTime, uint64_t currentFrame);

    void addBuffer(const std::string& name, uint32_t sizeBytes, const void* data);
    std::shared_ptr<Buffer> getBuffer(const std::string& name);

    std::shared_ptr<Mesh> getMesh(const std::string& path);
    std::shared_ptr<Texture> getTexture(const std::string& path);

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
    VkDescriptorSetLayout _defaultDescriptorSetLayout;

    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;

    VmaAllocator _allocator;

    ImGuiData _imGuiData;
    
    MeshManager _meshManager;
    BufferManager _bufferManager;
    TextureManager _textureManger;
};
