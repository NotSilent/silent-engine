#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include "DeferredRenderpass.h"
#include "Queue.h"
#include "DeferredLightningRenderpass.h"

class Image;
class DrawData;

// TODO: cleaner
struct FrameSynchronization {
    VkFence queueFence;
    VkSemaphore imageAcquireSemaphore;
    VkSemaphore presentSemaphore;

    explicit FrameSynchronization(VkDevice device);

    void destroy(VkDevice device);
};

class FrameResources {
public:
    FrameResources(VkDevice device,
                   VmaAllocator allocator,
                   uint32_t queueFamilyIndex,
                   Image swapchainImage,
                   VkDescriptorSet deferredLightningSet,
                   VkPipelineLayout deferredLightningPipelineLayout,
                   VkPipeline deferredLightningPipeline,
                   const VkRect2D &renderArea);

    ~FrameResources() = default;

    FrameResources(const FrameResources &other) = delete;

    FrameResources &operator=(const FrameResources &other) = delete;

    FrameResources(FrameResources &&other) noexcept;

    FrameResources &operator=(FrameResources &&other) noexcept;

    void renderFrame(VkSwapchainKHR swapchain, VkQueue graphicsQueue, uint32_t imageIndex, VkSemaphore imageAcquireSemaphore, VkFence presentFence, const DrawData& drawData);

    // Should be handled by whatever will create Images
    void destroy();

private:
    VkDevice device;

    Image swapchainImage;

    VkCommandPool cmdPool;
    VkCommandBuffer cmd;

    FrameSynchronization synchronization;

    DeferredRenderpass deferredRenderPass;
    DeferredLightningRenderpass deferredLightningRenderPass;
};
