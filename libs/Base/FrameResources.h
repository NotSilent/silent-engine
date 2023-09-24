#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

class Image;
class DrawData;

// TODO: cleaner
struct FrameSynchronization {
    VkFence queueFence;
    VkSemaphore imageAcquireSemaphore;
    VkSemaphore presentSemahore;

    explicit FrameSynchronization(VkDevice device);

    void destroy(VkDevice device);
};

class FrameResources {
private:
    VkDevice device;
    // TODO: remove together with image
    VmaAllocator allocator;

    uint32_t queueFamilyIndex;
    VkCommandPool cmdPool;
    VkCommandBuffer cmd;

    FrameSynchronization synchronization;

    VkImage _swapchainImage;
    VkImageView _swapchainImageView;

    // TODO:
    // Should be just a handle, from ImageManager
    // Default move constructors after changing to handle
    std::shared_ptr<Image> _colorImage;

public:
    FrameResources(VkDevice device,
                   VmaAllocator allocator,
                   uint32_t queueFamilyIndex,
                   VkImage swapchainImage,
                   VkImageView swapchainImageView,
                   const VkRect2D &renderArea);

    ~FrameResources() = default;

    FrameResources(const FrameResources &other) = delete;

    FrameResources &operator=(const FrameResources &other) = delete;

    FrameResources(FrameResources &&other) noexcept;

    FrameResources &operator=(FrameResources &&other) noexcept;

    void prepareNewFrame(VkSwapchainKHR swapchain, VkQueue graphicsQueue, uint32_t imageIndex, VkSemaphore imageAcquireSemaphore, const DrawData& drawData, VkRect2D renderArea);

    // Should be handled by whatever will create Images
    void destroy();
};
