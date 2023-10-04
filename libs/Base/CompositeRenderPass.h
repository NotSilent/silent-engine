#pragma once

#include <vulkan/vulkan_core.h>
#include "DeferredRenderPass.h"

class CompositeRenderPass {
public:
    // TODO: create pipeline
    CompositeRenderPass(VkDevice device, VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout, VkPipeline pipeline, VkRect2D renderArea, const DeferredRenderPassOutput& deferredRenderPassOutput);

    CompositeRenderPass(CompositeRenderPass& other) = delete;
    CompositeRenderPass& operator=(CompositeRenderPass& other) = delete;

    CompositeRenderPass(CompositeRenderPass&& other) = default;
    CompositeRenderPass& operator=(CompositeRenderPass&& other) = default;

    void render(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView);

private:
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkRect2D renderArea;

    DeferredRenderPassOutput deferredRenderPassOutput;

    VkDescriptorSet descriptorSet;

    // TODO: get from some manager
    VkSampler sampler;

    inline static VkClearValue swapchainClearValue = {0.25f, 0.25f, 0.25f, 0.25f};

    void beginRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView);
    void endRenderPass(VkCommandBuffer cmd, VkImage swapchainImage);

    VkSampler createSampler(VkDevice device);
};
