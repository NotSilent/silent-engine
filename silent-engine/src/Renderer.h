#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk-bootstrap/VkBootstrap.h"

#include "ImGuiData.h"
#include "Mesh.h"

#include "Camera.h"
#include "Image.h"
#include "MeshManager.h"
#include "Texture.h"
#include "TextureManager.h"

// TODO: VkCommandPool and VkCommands creation manager;

class Renderer {
public:

    void init(GLFWwindow* window);
    void cleanup();
    void draw();
    void run();

private:
    // TODO: Input manager
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    const std::string ENGINE_NAME = "Silent Engine";
    const std::string STANFORD_BUNNY_ASSET_LOCATION = "assets/stanford-bunny.obj";

    const std::string PINK_TEXTURE_ASSET_LOCATION = "assets/pink.png";
    const std::string TEST_TEXTURE_ASSET_LOCATION = "assets/test.png";

    const uint32_t WIDTH = 1920;
    const uint32_t HEIGHT = 1080;

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
    VkDescriptorSet _defaultDescriptorSet;

    VkPipelineLayout _pipelineLayout;
    VkPipeline _pipeline;

    VmaAllocator _allocator;

    ImGuiData _imGuiData;

    MeshManager _meshManager;
    TextureManager _textureManger;

    Camera _camera;
};
