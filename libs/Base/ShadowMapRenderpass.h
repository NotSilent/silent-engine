#pragma once

#include <vulkan/vulkan.hpp>
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
    ShadowMapRenderpass(vk::Device device, VmaAllocator allocator, const PipelineManager& pipelineManager);

    ShadowMapRenderpass(ShadowMapRenderpass &other) = delete;

    ShadowMapRenderpass &operator=(ShadowMapRenderpass &other) = delete;

    ShadowMapRenderpass(ShadowMapRenderpass &&other) = default;

    ShadowMapRenderpass &operator=(ShadowMapRenderpass &&other) = default;

    void render(vk::CommandBuffer cmd, const DrawData& drawData);

    void destroy();

    [[nodiscard]] ShadowMapRenderPassOutput getOutput() const;

private:
    inline static const uint32_t SHADOW_MAP_RESOLUTION = 2048;
    inline static const vk::ClearValue DEPTH_CLEAR_VALUE = vk::ClearDepthStencilValue{{1.0f, 0}};
    inline static const vk::Rect2D SHADOW_MAP_DIMENSIONS{
        {0, 0},
        {SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION}
    };

    vk::Device device;
    VmaAllocator allocator;

    Image depthImage;

    ShadowMapMaterial material;

    void beginRenderPass(vk::CommandBuffer cmd);

    void endRenderPass(vk::CommandBuffer cmd);

    [[nodiscard]] Image createDepthImage() const;
};