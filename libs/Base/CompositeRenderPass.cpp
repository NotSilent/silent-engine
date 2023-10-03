#include "CompositeRenderPass.h"
#include "CommandBuffer.h"
#include <array>

CompositeRenderPass::CompositeRenderPass(VkPipeline pipeline, VkRect2D renderArea)
    : pipeline(pipeline)
    , renderArea(renderArea) {

}

void CompositeRenderPass::render(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView,
                                 const DeferredRenderPassOutput &deferredRenderPassOutput) {
    beginRenderPass(cmd, swapchainImage, swapchainImageView, deferredRenderPassOutput);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdDraw(cmd, 3, 1, 0, 0);

    endRenderPass(cmd, swapchainImage);
}

void CompositeRenderPass::beginRenderPass(VkCommandBuffer cmd, VkImage swapchainImage, VkImageView swapchainImageView,
                                          const DeferredRenderPassOutput &deferredRenderPassOutput) {
    VkRenderingAttachmentInfo swapchainAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = swapchainImageView,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = swapchainClearValue,
    };

    VkRenderingAttachmentInfo colorAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = deferredRenderPassOutput.color.imageView,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .clearValue = {},
    };

    std::array attachments {swapchainAttachment, colorAttachment};

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   deferredRenderPassOutput.color.accessMask,
                                   VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
                                   deferredRenderPassOutput.color.imageLayout,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = renderArea,
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = attachments.size(),
            .pColorAttachments = attachments.data(),
            .pDepthAttachment = nullptr,
            .pStencilAttachment = nullptr,
    };

    vkCmdBeginRendering(cmd, &renderingInfo);
}

void CompositeRenderPass::endRenderPass(VkCommandBuffer cmd, VkImage swapchainImage) {
    vkCmdEndRendering(cmd);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_ACCESS_NONE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);
}
