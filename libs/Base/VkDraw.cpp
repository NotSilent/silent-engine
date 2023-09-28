#include "VkDraw.h"
#include "VkInit.h"
#include "PushData.h"
#include "CommandBuffer.h"

// TODO: part of gbuffer?
VkClearValue clearValues[]{
        {0.75f, 0.75f, 0.75f},
        {0.0f,  0.0f,  0.0f},
        {0.0f,  0.0f,  0.0f},
        {0.0f,  0.0f,  0.0f},
        {1.0f,  0},
};

void VkDraw::recordCommandBuffer(VkCommandBuffer cmd, const DrawData &drawData,
                                            VkImage swapchainImage,
                                            VkImageView swapchainImageView,
                                            uint32_t graphicsFamilyIndex,
                                            const VkRect2D &renderArea) {
    VkClearValue clearValue{1.0f, 0.0f, 1.0f, 1.0f};

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

    VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
    };

    vkBeginCommandBuffer(cmd, &beginInfo);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_ACCESS_NONE,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   graphicsFamilyIndex,
                                   graphicsFamilyIndex,
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

    for (const DrawCall& drawCall: drawData.getDrawCalls()) {
        PushData pushData(glm::mat4(1.0f), drawData.view, drawData.projection);

        vkCmdPushConstants(cmd, drawCall.pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushData), &pushData);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall.pipeline);

        VkDeviceSize offset = 0;
        VkBuffer vertexBuffer = drawCall._mesh->getVertexBuffer();
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);

        vkCmdBindIndexBuffer(cmd, drawCall._mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, drawCall._mesh->getIndexCount(), 1, 0, 0, 0);
    }

    vkCmdEndRendering(cmd);

    CommandBuffer::pipelineBarrier(cmd, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                                   VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                                   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                                   VK_ACCESS_NONE,
                                   VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                   VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                   graphicsFamilyIndex,
                                   graphicsFamilyIndex,
                                   swapchainImage,
                                   VK_IMAGE_ASPECT_COLOR_BIT);

    vkEndCommandBuffer(cmd);

    // Setup frame
    // pipelineLayout
    // vkCmdBindDescriptorSet(frameSet)

    // Setup render pass
    // pipelineLayout
    // vkCmdBindDescriptorSet(renderPassSet)

    // Setup pipeline
    // vkCmdBindPipeline(pipeline)
    // vkCmdBindDescriptorSet(pipelineSet)

    // Setup material
    // vkCmdBindDescriptorSet(materialSet)
    // vkCmdBindPipeline(materialPipeline)

    // Setup materialInstance?
    // vkCmdBindDescriptorSet(materialInstanceSet)

    // Setup object
    // vkCmdBindVertexBuffer(objectBuffer)
    // vkCmdBindIndexBuffer(objectBuffer)
}
