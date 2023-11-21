#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>
#include "DeferredRenderpass.h"
#include "Queue.h"
#include "DeferredLightningRenderpass.h"
#include "PipelineManager.h"
#include "ShadowMapRenderpass.h"

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
                   PipelineManager& pipelineManager,
                   const VkRect2D &renderArea);

    ~FrameResources() = default;

    FrameResources(const FrameResources &other) = delete;

    FrameResources &operator=(const FrameResources &other) = delete;

    FrameResources(FrameResources &&other) = default;

    FrameResources &operator=(FrameResources &&other) = default;

    void renderFrame(VkSwapchainKHR swapchain, VkQueue graphicsQueue, uint32_t imageIndex, VkSemaphore imageAcquireSemaphore, VkFence presentFence, const DrawData& drawData);

    void destroy();

private:
    VkDevice device;

    Image swapchainImage;

    VkCommandPool cmdPool;
    VkCommandBuffer cmd;

    FrameSynchronization synchronization;

    ShadowMapRenderpass shadowMapRenderpass;

    DeferredRenderpass deferredRenderPass;
    DeferredLightningRenderpass deferredLightningRenderPass;
};
