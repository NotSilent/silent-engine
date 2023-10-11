#include "DeferredLightningRenderpass.h"
#include "CommandBuffer.h"

DeferredLightningRenderpass::DeferredLightningRenderpass(PipelineManager& pipelineManager, VkRect2D renderArea, const DeferredRenderPassOutput& deferredRenderPassOutput)
    : renderArea(renderArea)
    , deferredRenderPassOutput(deferredRenderPassOutput)
    , material(pipelineManager.createDeferredLightningMaterial(deferredRenderPassOutput.color.imageView, deferredRenderPassOutput.normal.imageView, deferredRenderPassOutput.position.imageView)) {
}

void DeferredLightningRenderpass::render(VkCommandBuffer cmd, const Image& swapchainImage, glm::vec3 viewDirection) {
    beginRenderPass(cmd, swapchainImage);

    vkCmdPushConstants(cmd, material.layout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3), &viewDirection);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.layout, 0, 1, &material.set, 0, nullptr);

    vkCmdDraw(cmd, 6, 1, 0, 0);

    endRenderPass(cmd, swapchainImage);
}

void DeferredLightningRenderpass::beginRenderPass(VkCommandBuffer cmd, const Image& swapchainImage) {
    VkRenderingAttachmentInfo swapchainAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = swapchainImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = swapchainClearValue,
    };

    std::array attachments {swapchainAttachment};

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   swapchainImage.getImage(),
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_SHADER_READ_BIT,
                                   deferredRenderPassOutput.color.imageLayout,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   deferredRenderPassOutput.color.image,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_SHADER_READ_BIT,
                                   deferredRenderPassOutput.normal.imageLayout,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   deferredRenderPassOutput.normal.image,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_SHADER_READ_BIT,
                                   deferredRenderPassOutput.position.imageLayout,
                                   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                   deferredRenderPassOutput.position.image,
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

void DeferredLightningRenderpass::endRenderPass(VkCommandBuffer cmd, const Image& swapchainImage) {
    vkCmdEndRendering(cmd);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_ACCESS_NONE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                   swapchainImage.getImage(),
                                   VK_IMAGE_ASPECT_COLOR_BIT);
}
