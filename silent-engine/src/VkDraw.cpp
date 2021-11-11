#include "VkDraw.h"
#include "VkInit.h"
#include <Material.h>
#include <PushData.h>

VkClearValue clearValues[] {
    { 0.75f, 0.75f, 0.75f },
    { 1.0f, 0 },
};

VkCommandBuffer VkDraw::recordCommandBuffer(vkb::Device& device, VkCommandPool commandPool, const DrawData& drawData, VkRenderPass renderPass, VkFramebuffer framebuffer, const VkRect2D& renderArea, const ImGuiData& imGuiData)
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

        std::vector<VkBuffer> vertexBuffers;
        std::vector<VkDeviceSize> offsets;
        for(auto& attribute : drawCall.mesh->getAttributes()) {
            vertexBuffers.push_back(attribute.buffer->getBuffer());
            offsets.push_back(0);
        }

        std::shared_ptr<Material> material = drawCall.material;
        VkDescriptorSet descriptorSet = material->getDescriptorSet();

        VkBuffer indexBuffer = drawCall.mesh->getIndexBuffer();
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->getPipeline());
        vkCmdPushConstants(cmd, material->getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushData), &pushData);
        vkCmdBindVertexBuffers(cmd, 0, vertexBuffers.size(), vertexBuffers.data(), offsets.data());
        vkCmdBindIndexBuffer(cmd, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, drawCall.mesh->getIndexCount(), 1, 0, 0, 0);
    }

    imGuiData.appendDrawToCommandBuffer(cmd);

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

    return cmd;
}
