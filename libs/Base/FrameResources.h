#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.hpp>
#include "DeferredRenderpass.h"
#include "Queue.h"
#include "DeferredLightningRenderpass.h"
#include "PipelineManager.h"
#include "ShadowMapRenderpass.h"

class Image;
class DrawData;

// TODO: cleaner
struct FrameSynchronization {
    vk::Fence queueFence;
    vk::Semaphore imageAcquireSemaphore;
    vk::Semaphore presentSemaphore;

    explicit FrameSynchronization(vk::Device device);

    void destroy(vk::Device device);
};

class FrameResources {
public:
    FrameResources(vk::Device device,
                   VmaAllocator allocator,
                   uint32_t queueFamilyIndex,
                   Image swapchainImage,
                   PipelineManager& pipelineManager,
                   const vk::Rect2D &renderArea);

    ~FrameResources() = default;

    FrameResources(const FrameResources &other) = delete;

    FrameResources &operator=(const FrameResources &other) = delete;

    FrameResources(FrameResources &&other) = default;

    FrameResources &operator=(FrameResources &&other) = default;

    void renderFrame(vk::SwapchainKHR swapchain, vk::Queue graphicsQueue, uint32_t imageIndex, vk::Semaphore imageAcquireSemaphore, vk::Fence presentFence, const DrawData& drawData);

    void destroy();

private:
    vk::Device device;

    Image swapchainImage;

    vk::CommandPool cmdPool;
    vk::CommandBuffer cmd;

    FrameSynchronization synchronization;

    ShadowMapRenderpass shadowMapRenderpass;

    DeferredRenderpass deferredRenderPass;
    DeferredLightningRenderpass deferredLightningRenderPass;
};
