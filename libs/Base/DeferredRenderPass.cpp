#include "DeferredRenderPass.h"

#include "CommandBuffer.h"

DeferredRenderPass::DeferredRenderPass(VkDevice device,
                                       VmaAllocator allocator,
                                       VkRect2D renderArea,
                                       VkImage swapchainImage,
                                       VkImageView swapchainImageView)
        : device(device)
        , allocator(allocator)
        , renderArea(renderArea)
        , swapchainImage(swapchainImage)
        , swapchainImageView(swapchainImageView) {
    colorImage = createColorImage();
}

void DeferredRenderPass::destroy() {
    colorImage.destroy(device, allocator);
}

void DeferredRenderPass::render(VkCommandBuffer cmd,
                                const std::function<void(VkCommandBuffer cmd)> &renderPassRecording) {
    beginRenderPass(cmd);

    renderPassRecording(cmd);

    endRenderPass(cmd);
}

void DeferredRenderPass::beginRenderPass(VkCommandBuffer cmd) {
    VkRenderingAttachmentInfo colorAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = colorImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = clearValue2,
    };

    VkRenderingAttachmentInfo swapchainAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = swapchainImageView,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = clearValue,
    };

    std::array attachments {colorAttachment, swapchainAttachment};

    // TODO: Single vkCmdPipelineBarrier

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   colorImage.getImage(),
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
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

void DeferredRenderPass::endRenderPass(VkCommandBuffer cmd) {
    vkCmdEndRendering(cmd);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_ACCESS_SHADER_READ_BIT,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
                                   colorImage.getImage(),
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_ACCESS_NONE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);
}

Image DeferredRenderPass::createColorImage() const {
    ImageCreateInfo createInfo {
            .extent = {renderArea.extent.width, renderArea.extent.height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };

    return Image(device, allocator, createInfo);
}
