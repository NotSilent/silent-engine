#include "DeferredRenderpass.h"

#include "CommandBuffer.h"
#include "PushData.h"
#include "DrawData.h"
#include "RenderPassAttachmentOutput.h"

DeferredRenderpass::DeferredRenderpass(VkDevice device,
                                       VmaAllocator allocator,
                                       VkRect2D renderArea)
        : device(device)
        , allocator(allocator)
        , renderArea(renderArea) {
    colorImage = createColorImage(DeferredRenderpassDefinitions::Formats::COLOR);
    normalImage = createColorImage(DeferredRenderpassDefinitions::Formats::NORMAL);
    positionImage = createColorImage(DeferredRenderpassDefinitions::Formats::POSITION);
    depthImage = createDepthImage();
}

void DeferredRenderpass::destroy() {
    colorImage.destroy(device, allocator);
    normalImage.destroy(device, allocator);
    positionImage.destroy(device, allocator);
    depthImage.destroy(device, allocator);
}

void DeferredRenderpass::render(VkCommandBuffer cmd, const DrawData& drawData) {
    beginRenderPass(cmd);

    // TODO: Why tf is deferredPipelineLayout a part of DrawData

    for (const DrawCall &drawCall: drawData.getDrawCalls()) {
        PushData pushData(drawCall.model, drawData.view, drawData.projection);
        vkCmdPushConstants(cmd, drawData.deferredPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(PushData),
                           &pushData);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall.pipeline);

        std::array buffers {
                drawCall._mesh->getPositionsBuffer(),
                drawCall._mesh->getAttributesBuffer(),
        };

        std::array<VkDeviceSize, 2> offsets {
                0,
                0,
        };

        vkCmdBindVertexBuffers(cmd, 0, buffers.size(), buffers.data(), offsets.data());

        vkCmdBindIndexBuffer(cmd, drawCall._mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, drawCall._mesh->getIndexCount(), 1, 0, 0, 0);
    }

    endRenderPass(cmd);
}

void DeferredRenderpass::beginRenderPass(VkCommandBuffer cmd) {
    VkRenderingAttachmentInfo colorAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = colorImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = CLEAR_VALUE,
    };

    VkRenderingAttachmentInfo normalAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = normalImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = CLEAR_VALUE,
    };

    VkRenderingAttachmentInfo positionAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = positionImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = CLEAR_VALUE,
    };

    VkRenderingAttachmentInfo depthAttachment {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = depthImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = nullptr,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .clearValue = DEPTH_CLEAR_VALUE,
    };

    std::array attachments {colorAttachment, normalAttachment, positionAttachment};

    // TODO: Single vkCmdPipelineBarrier

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   colorImage.getImage(),
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   normalImage.getImage(),
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   positionImage.getImage(),
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
                                   depthImage.getImage(),
                                   VK_IMAGE_ASPECT_DEPTH_BIT);

    VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = renderArea,
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = attachments.size(),
            .pColorAttachments = attachments.data(),
            .pDepthAttachment = &depthAttachment,
            .pStencilAttachment = nullptr,
    };

    vkCmdBeginRendering(cmd, &renderingInfo);
}

void DeferredRenderpass::endRenderPass(VkCommandBuffer cmd) {
    vkCmdEndRendering(cmd);
}

Image DeferredRenderpass::createColorImage(VkFormat format) const {
    ImageCreateInfo createInfo {
            .extent = {renderArea.extent.width, renderArea.extent.height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };

    return Image(device, allocator, createInfo);
}

Image DeferredRenderpass::createDepthImage() const {
    ImageCreateInfo createInfo {
            .extent = {renderArea.extent.width, renderArea.extent.height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = DeferredRenderpassDefinitions::Formats::DEPTH,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
    };

    return Image(device, allocator, createInfo);
}

DeferredRenderPassOutput DeferredRenderpass::getOutput() const {
    RenderPassAttachmentOutput color = {
            .image = colorImage.getImage(),
            .imageView = colorImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    RenderPassAttachmentOutput normal = {
            .image = normalImage.getImage(),
            .imageView = normalImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    RenderPassAttachmentOutput position = {
            .image = positionImage.getImage(),
            .imageView = positionImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    RenderPassAttachmentOutput depth = {
            .image = depthImage.getImage(),
            .imageView = depthImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    };

    return DeferredRenderPassOutput {
        .color = color,
        .normal = normal,
        .position = position,
        .depth = depth,
    };
}
