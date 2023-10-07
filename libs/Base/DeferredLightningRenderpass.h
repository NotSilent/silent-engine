#pragma once

#include <vulkan/vulkan_core.h>
#include "DeferredRenderpass.h"

class DeferredLightningRenderpass {
public:
    // TODO: create pipeline
    DeferredLightningRenderpass(VkDevice device, VkDescriptorSet descriptorSet, VkPipelineLayout pipelineLayout, VkPipeline pipeline, VkRect2D renderArea, const DeferredRenderPassOutput& deferredRenderPassOutput);

    DeferredLightningRenderpass(DeferredLightningRenderpass& other) = delete;
    DeferredLightningRenderpass& operator=(DeferredLightningRenderpass& other) = delete;

    DeferredLightningRenderpass(DeferredLightningRenderpass&& other) = default;
    DeferredLightningRenderpass& operator=(DeferredLightningRenderpass&& other) = default;

    void render(VkCommandBuffer cmd, const Image& swapchainImage);

private:
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkRect2D renderArea;

    DeferredRenderPassOutput deferredRenderPassOutput;

    VkDescriptorSet descriptorSet;

    // TODO: get from some manager
    VkSampler sampler;

    inline static VkClearValue swapchainClearValue = {0.0f, 0.0f, 0.0f, 0.0f};

    void beginRenderPass(VkCommandBuffer cmd, const Image& swapchainImage);
    void endRenderPass(VkCommandBuffer cmd, const Image& swapchainImage);

    VkSampler createSampler(VkDevice device);
};
