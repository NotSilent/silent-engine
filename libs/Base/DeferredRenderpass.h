#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
#include "Image.h"

namespace DeferredRenderpassDefinitions{
    namespace Formats {
        static inline VkFormat COLOR = VK_FORMAT_R8G8B8A8_UNORM;
        static inline VkFormat NORMAL = VK_FORMAT_R16G16B16A16_SFLOAT;
        static inline VkFormat POSITION = VK_FORMAT_R16G16B16A16_SFLOAT;
        static inline VkFormat DEPTH = VK_FORMAT_D32_SFLOAT;
    }
};

struct RenderPassAttachmentOutput {
    VkImage image;
    VkImageView imageView;
    VkImageLayout imageLayout;
};

struct DeferredRenderPassOutput {
    RenderPassAttachmentOutput color;
    RenderPassAttachmentOutput normal;
    RenderPassAttachmentOutput position;
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

    static inline VkClearValue clearValue{0.0f, 0.0f, 0.0f, 0.0f};
    static inline VkClearValue depthClearValue {1.0f, 0};

    VkRect2D renderArea;

    Image colorImage;
    Image normalImage;
    Image positionImage;

    Image depthImage;

    void beginRenderPass(VkCommandBuffer cmd);
    void endRenderPass(VkCommandBuffer cmd);

    [[nodiscard]] Image createColorImage(VkFormat format) const;

    [[nodiscard]] Image createDepthImage() const;
};