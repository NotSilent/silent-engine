#pragma once

#include <vulkan/vulkan.hpp>
#include <functional>
#include "Image.h"
#include "RenderPassAttachmentOutput.h"

class DrawData;

namespace DeferredRenderpassDefinitions::Formats {
    static inline vk::Format COLOR = vk::Format::eR8G8B8A8Unorm;
    static inline vk::Format NORMAL = vk::Format::eR16G16B16A16Sfloat;
    static inline vk::Format POSITION = vk::Format::eR16G16B16A16Sfloat;
    static inline vk::Format DEPTH = vk::Format::eD32Sfloat;
}

struct DeferredRenderPassOutput {
    RenderPassAttachmentOutput color;
    RenderPassAttachmentOutput normal;
    RenderPassAttachmentOutput position;
    RenderPassAttachmentOutput depth;
};

class DeferredRenderpass {
public:
    DeferredRenderpass(vk::Device device,
                       VmaAllocator allocator,
                       vk::Rect2D renderArea);

    DeferredRenderpass(DeferredRenderpass &other) = delete;

    DeferredRenderpass &operator=(DeferredRenderpass &other) = delete;

    DeferredRenderpass(DeferredRenderpass &&other) = default;

    DeferredRenderpass &operator=(DeferredRenderpass &&other) = default;

    void destroy();

    void render(vk::CommandBuffer cmd, const DrawData &drawData);

    [[nodiscard]] DeferredRenderPassOutput getOutput() const;

private:
    vk::Device device;
    VmaAllocator allocator;

    static inline const vk::ClearValue CLEAR_VALUE = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 0.0f);
    static inline const vk::ClearValue DEPTH_CLEAR_VALUE = vk::ClearDepthStencilValue(1.0f, 0);

    vk::Rect2D renderArea;

    Image colorImage;
    Image normalImage;
    Image positionImage;

    Image depthImage;

    void beginRenderPass(vk::CommandBuffer cmd);

    void endRenderPass(vk::CommandBuffer cmd);

    [[nodiscard]] Image createColorImage(vk::Format format) const;

    [[nodiscard]] Image createDepthImage() const;
};