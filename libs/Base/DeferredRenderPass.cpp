#include "DeferredRenderPass.h"

#include <utility>
#include "CommandBuffer.h"

DeferredRenderPass::DeferredRenderPass(VkRect2D renderArea
                                       , VkImage swapchainImage
                                       , VkImageView swapchainImageView)
                                       : renderArea(renderArea)
                                       , swapchainImage(swapchainImage)
                                       , swapchainImageView(swapchainImageView) {

}

void DeferredRenderPass::render(VkCommandBuffer cmd,
                                uint32_t graphicsQueueFamilyIndex,
                                const std::function<void(VkCommandBuffer cmd)>& renderPassRecording) {
    beginRenderPass(cmd, swapchainImage, swapchainImageView, graphicsQueueFamilyIndex);

    renderPassRecording(cmd);

    endRenderPass(cmd, swapchainImage, graphicsQueueFamilyIndex);
}

void DeferredRenderPass::beginRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView, uint32_t graphicsQueueFamilyIndex) {

    VkRenderingAttachmentInfo colorAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = swapchainImageView,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = clearValue,
    };

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   graphicsQueueFamilyIndex,
                                   graphicsQueueFamilyIndex,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .pNext = nullptr,
            .flags = {},
            .renderArea = renderArea,
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr,
    };

    vkCmdBeginRendering(cmd, &renderingInfo);
}

void DeferredRenderPass::endRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, uint32_t graphicsQueueFamilyIndex) {
    vkCmdEndRendering(cmd);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_ACCESS_NONE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                   graphicsQueueFamilyIndex,
                                   graphicsQueueFamilyIndex,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);
}
