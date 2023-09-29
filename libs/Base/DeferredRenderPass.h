#pragma once

#include <vulkan/vulkan_core.h>
#include <functional>

class DeferredRenderPass {
public:
    DeferredRenderPass(VkRect2D renderArea, VkImage swapchainImage, VkImageView swapchainImageView);

    DeferredRenderPass(DeferredRenderPass& other) = delete;
    DeferredRenderPass& operator=(DeferredRenderPass& other) = delete;

    DeferredRenderPass(DeferredRenderPass&& other) = default;
    DeferredRenderPass& operator=(DeferredRenderPass&& other) = default;

    void render(VkCommandBuffer cmd,
                uint32_t graphicsQueueFamilyIndex,
                const std::function<void(VkCommandBuffer cmd)>& renderPassRecording);

private:
    static inline VkClearValue clearValue{1.0f, 0.0f, 1.0f, 1.0f};

    VkRect2D renderArea;

    VkImage swapchainImage;
    VkImageView swapchainImageView;

    void beginRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView, uint32_t graphicsQueueFamilyIndex);
    void endRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, uint32_t graphicsQueueFamilyIndex);
};