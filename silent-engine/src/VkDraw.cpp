#include "VkDraw.h"

VkClearValue clearValues[] {
    { 0.75f, 0.75f, 0.75f },
    { 1.0f, 0 },
};

VkCommandBuffer VkDraw::recordCommandBuffer(VkDevice device, VkCommandPool commandPool, const DrawData& drawData, VkPipelineLayout pipelineLayout, VkPipeline pipeline, VkRenderPass renderPass, VkFramebuffer framebuffer, const VkRect2D& renderArea, const ImGuiData& imGuiData, VkDescriptorSet descriptorSet)
{
    VkRenderPassBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = framebuffer,
        .renderArea = renderArea,
        .clearValueCount = static_cast<uint32_t>(std::size(clearValues)),
        .pClearValues = clearValues,
    };

    VkCommandBufferBeginInfo cmdBeginInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    VkCommandBufferAllocateInfo allocateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(device, &allocateInfo, &cmd);

    vkBeginCommandBuffer(cmd, &cmdBeginInfo);

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    for (auto& drawCall : drawData.getDrawCalls()) {
        PushData pushData {
            .model = drawCall.model,
            .view = drawData.getCamera().getViewMatrix(),
            .projection = drawData.getCamera().getProjectionMatrix(),
            .viewPosition = drawData.getCamera().getPosition(),
        };

        VkDeviceSize offset { 0 };
        VkBuffer vertexBuffer = drawCall.mesh->getVertexBuffer();
        VkBuffer indexBuffer = drawCall.mesh->getIndexBuffer();
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushData), &pushData);
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        VkDescriptorImageInfo imageInfo {
            .sampler = drawCall.texture->getSampler(),
            .imageView = drawCall.texture->getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        VkWriteDescriptorSet write {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = descriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

        vkCmdDrawIndexed(cmd, drawCall.mesh->getIndexCount(), 1, 0, 0, 0);
    }

    imGuiData.appendDrawToCommandBuffer(cmd);

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

    return cmd;
}
