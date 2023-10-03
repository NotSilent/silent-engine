#pragma once

#include <vulkan/vulkan_core.h>
#include "DeferredRenderPass.h"

class CompositeRenderPass {
public:
    // TODO: create pipeline
    CompositeRenderPass(VkPipeline pipeline, VkRect2D renderArea);

    CompositeRenderPass(CompositeRenderPass& other) = delete;
    CompositeRenderPass& operator=(CompositeRenderPass& other) = delete;

    CompositeRenderPass(CompositeRenderPass&& other) = default;
    CompositeRenderPass& operator=(CompositeRenderPass&& other) = default;

    void render(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView, const DeferredRenderPassOutput& deferredRenderPassOutput);

private:
    VkPipeline pipeline;
    VkRect2D renderArea;

    inline static VkClearValue swapchainClearValue = {1.0f, 0.0f, 0.0f, 0.0f};

    void beginRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView,
                         const DeferredRenderPassOutput &deferredRenderPassOutput);
    void endRenderPass(VkCommandBuffer cmd, VkImage swapchainImage);
};
