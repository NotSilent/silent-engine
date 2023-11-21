#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
#include "vk_mem_alloc.h"
#include "Image.h"
#include "ShadowMapMaterial.h"
#include "RenderPassAttachmentOutput.h"

class PipelineManager;
class DrawData;

struct ShadowMapRenderPassOutput {
    RenderPassAttachmentOutput depth;
};

class ShadowMapRenderpass {
public:
    ShadowMapRenderpass(VkDevice device, VmaAllocator allocator, const PipelineManager& pipelineManager);

    ShadowMapRenderpass(ShadowMapRenderpass &other) = delete;

    ShadowMapRenderpass &operator=(ShadowMapRenderpass &other) = delete;

    ShadowMapRenderpass(ShadowMapRenderpass &&other) = default;

    ShadowMapRenderpass &operator=(ShadowMapRenderpass &&other) = default;

    void render(VkCommandBuffer cmd, const DrawData& drawData);

    void destroy();

    [[nodiscard]] ShadowMapRenderPassOutput getOutput() const;

private:
    inline static const uint32_t SHADOW_MAP_RESOLUTION = 2048;
    inline static const VkClearValue DEPTH_CLEAR_VALUE = {
            .depthStencil {1.0f, 0}
    };
    inline static const VkRect2D SHADOW_MAP_DIMENSIONS{
            .offset = {0, 0},
            .extent = {SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION}
    };

    VkDevice device;
    VmaAllocator allocator;

    Image depthImage;

    ShadowMapMaterial material;

    void beginRenderPass(VkCommandBuffer cmd);

    void endRenderPass(VkCommandBuffer cmd);

    [[nodiscard]] Image createDepthImage() const;
};