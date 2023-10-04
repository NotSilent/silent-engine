#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
#include "Image.h"

struct RenderPassAttachmentOutput {
    VkImage image;
    VkImageView imageView;
    VkAccessFlags accessMask;
    VkImageLayout imageLayout;
};

struct DeferredRenderPassOutput {
    RenderPassAttachmentOutput color;
    RenderPassAttachmentOutput depth;
};

class DeferredRenderpass {
public:
    DeferredRenderpass(VkDevice device,
                       VmaAllocator allocator,
                       VkRect2D renderArea);

    DeferredRenderpass(DeferredRenderpass& other) = delete;
    DeferredRenderpass& operator=(DeferredRenderpass& other) = delete;

    DeferredRenderpass(DeferredRenderpass&& other) = default;
    DeferredRenderpass& operator=(DeferredRenderpass&& other) = default;

    void destroy();

    void render(VkCommandBuffer cmd,
                const std::function<void(VkCommandBuffer cmd)>& renderPassRecording);

    [[nodiscard]] DeferredRenderPassOutput getOutput() const;

private:
    VkDevice device;
    VmaAllocator allocator;

    static inline VkClearValue clearValue{1.0f, 0.0f, 1.0f, 1.0f};
    static inline VkClearValue depthClearValue {1.0f, 0};

    VkRect2D renderArea;

    Image colorImage;

    Image depthImage;

    void beginRenderPass(VkCommandBuffer cmd);
    void endRenderPass(VkCommandBuffer cmd);

    [[nodiscard]] Image createColorImage() const;

    [[nodiscard]] Image createDepthImage() const;
};