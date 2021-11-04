#include "VkDraw.h"
#include "VkInit.h"

VkClearValue clearValues[] {
    { 0.75f, 0.75f, 0.75f },
    { 1.0f, 0 },
};

VkCommandBuffer VkDraw::recordCommandBuffer(vkb::Device& device, VkCommandPool commandPool, const DrawData& drawData, VkPipelineLayout pipelineLayout, VkPipeline pipeline, VkRenderPass renderPass, VkFramebuffer framebuffer, const VkRect2D& renderArea, const ImGuiData& imGuiData, VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout)
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
    vkAllocateCommandBuffers(device.device, &allocateInfo, &cmd);

    vkBeginCommandBuffer(cmd, &cmdBeginInfo);

    vkCmdBeginRenderPass(cmd, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);

    for (auto& drawCall : drawData.getDrawCalls()) {
        PushData pushData {
            .model = drawCall.model,
            .view = drawData.getCamera()->getViewMatrix(),
            .projection = drawData.getCamera()->getProjectionMatrix(),
            .viewPosition = drawData.getCamera()->getPosition(),
        };

        VkDeviceSize offset { 0 };
        VkBuffer vertexBuffer = drawCall.mesh->getVertexBuffer();
        VkBuffer indexBuffer = drawCall.mesh->getIndexBuffer();
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushData), &pushData);
        vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer, &offset);
        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        VkDescriptorSet descriptorSet = VkInit::createDescriptorSet(device, descriptorPool, descriptorSetLayout, drawCall.texture->getSampler(), drawCall.texture->getImageView());
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, drawCall.mesh->getIndexCount(), 1, 0, 0, 0);
    }

    imGuiData.appendDrawToCommandBuffer(cmd);

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

    return cmd;
}
