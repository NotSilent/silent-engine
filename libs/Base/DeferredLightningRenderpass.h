#pragma once

#include <vulkan/vulkan.hpp>
#include "DeferredRenderpass.h"
#include "PipelineManager.h"
#include "DeferredLightningMaterial.h"
#include "ShadowMapRenderpass.h"
#include "glm/glm.hpp"

class DeferredLightningRenderpass {
public:
    // TODO: Pass PipelineManager as part of some kind of Engine/Renderer Context
    DeferredLightningRenderpass(PipelineManager& pipelineManager,
                                vk::Rect2D renderArea,
                                const DeferredRenderPassOutput& deferredRenderPassOutput,
                                const ShadowMapRenderPassOutput& shadowMapRenderPassOutput);

    DeferredLightningRenderpass(DeferredLightningRenderpass& other) = delete;
    DeferredLightningRenderpass& operator=(DeferredLightningRenderpass& other) = delete;

    DeferredLightningRenderpass(DeferredLightningRenderpass&& other) = default;
    DeferredLightningRenderpass& operator=(DeferredLightningRenderpass&& other) = default;

    void render(vk::CommandBuffer cmd, const Image& swapchainImage, const glm::mat4& lightSpace, glm::vec3 viewDirection);

private:
    // TODO: As parameter to render?
    vk::Rect2D renderArea;

    DeferredRenderPassOutput deferredRenderPassOutput;
    ShadowMapRenderPassOutput shadowMapRenderPassOutput;

    DeferredLightningMaterial material;

    inline static vk::ClearValue swapchainClearValue = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f);

    void beginRenderPass(vk::CommandBuffer cmd, const Image& swapchainImage);
    void endRenderPass(vk::CommandBuffer cmd, const Image& swapchainImage);
};
