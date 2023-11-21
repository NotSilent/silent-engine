#include "ShadowMapRenderpass.h"
#include "CommandBuffer.h"
#include "DeferredRenderpass.h"
#include "DrawData.h"
#include "PushData.h"
#include "PipelineManager.h"

ShadowMapRenderpass::ShadowMapRenderpass(VkDevice device, VmaAllocator allocator, const PipelineManager &pipelineManager)
        : device(device), allocator(allocator), material(pipelineManager.getShadowMapMaterial()) {
    depthImage = createDepthImage();
}

void ShadowMapRenderpass::destroy() {
    depthImage.destroy(device, allocator);
}

void ShadowMapRenderpass::beginRenderPass(VkCommandBuffer cmd) {
    VkRenderingAttachmentInfo depthAttachment{
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
            .renderArea = SHADOW_MAP_DIMENSIONS,
            .layerCount = 1,
            .viewMask = 0,
            .colorAttachmentCount = 0,
            .pColorAttachments = nullptr,
            .pDepthAttachment = &depthAttachment,
            .pStencilAttachment = nullptr,
    };

    vkCmdBeginRendering(cmd, &renderingInfo);
}

void ShadowMapRenderpass::endRenderPass(VkCommandBuffer cmd) {
    vkCmdEndRendering(cmd);
}

void ShadowMapRenderpass::render(VkCommandBuffer cmd, const DrawData &drawData) {
    beginRenderPass(cmd);

    // Shadow PushData
    // Uniform

    const glm::mat4 view = drawData.directionalLight.getView();
    const glm::mat4 projection = drawData.directionalLight.getProjection();
    for (const DrawCall &drawCall: drawData.getDrawCalls()) {
        PushData pushData(drawCall.model, view, projection);
        vkCmdPushConstants(cmd, material.layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                           sizeof(PushData),
                           &pushData);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);

        std::array buffers{
                drawCall._mesh->getPositionsBuffer(),
        };

        std::array<VkDeviceSize, 2> offsets{
                0,
        };

        vkCmdBindVertexBuffers(cmd, 0, buffers.size(), buffers.data(), offsets.data());

        vkCmdBindIndexBuffer(cmd, drawCall._mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, drawCall._mesh->getIndexCount(), 1, 0, 0, 0);
    }

    endRenderPass(cmd);
}

Image ShadowMapRenderpass::createDepthImage() const {
    ImageCreateInfo createInfo{
            .extent = {SHADOW_MAP_DIMENSIONS.extent.width, SHADOW_MAP_DIMENSIONS.extent.height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = DeferredRenderpassDefinitions::Formats::DEPTH,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
    };

    return Image(device, allocator, createInfo);
}

ShadowMapRenderPassOutput ShadowMapRenderpass::getOutput() const {
    RenderPassAttachmentOutput depth = {
            .image = depthImage.getImage(),
            .imageView = depthImage.getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    };

    return ShadowMapRenderPassOutput {
            .depth = depth,
    };
}
