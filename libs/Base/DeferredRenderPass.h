#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>
#include "Image.h"

class DeferredRenderPass {
public:
    DeferredRenderPass(VkDevice device,
                       VmaAllocator allocator,
                       VkRect2D renderArea,
                       VkImage swapchainImage,
                       VkImageView swapchainImageView);

    DeferredRenderPass(DeferredRenderPass& other) = delete;
    DeferredRenderPass& operator=(DeferredRenderPass& other) = delete;

    DeferredRenderPass(DeferredRenderPass&& other) = default;
    DeferredRenderPass& operator=(DeferredRenderPass&& other) = default;

    void destroy();

    void render(VkCommandBuffer cmd,
                const std::function<void(VkCommandBuffer cmd)>& renderPassRecording);

private:
    VkDevice device;
    VmaAllocator allocator;

    static inline VkClearValue clearValue{1.0f, 0.0f, 1.0f, 1.0f};
    static inline VkClearValue clearValue2{0.0f, 1.0f, 0.0f, 1.0f};

    VkRect2D renderArea;

    VkImage swapchainImage;
    VkImageView swapchainImageView;

    Image colorImage;

    void beginRenderPass(VkCommandBuffer cmd);
    void endRenderPass(VkCommandBuffer cmd);

    [[nodiscard]] Image createColorImage() const;
};