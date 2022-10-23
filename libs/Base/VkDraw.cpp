#include "VkDraw.h"
#include "VkInit.h"
#include "Material.h"
#include "PushData.h"
#include "DescriptorSet.h"

// TODO: part of gbuffer?
VkClearValue clearValues[]{
        {0.75f, 0.75f, 0.75f},
        {0.0f,  0.0f,  0.0f},
        {0.0f,  0.0f,  0.0f},
        {0.0f,  0.0f,  0.0f},
        {1.0f,  0},
};

VkCommandBuffer VkDraw::recordCommandBuffer(vkb::Device &device, VkCommandPool commandPool, const DrawData &drawData,
                                            VkRenderPass renderPass, VkFramebuffer framebuffer,
                                            const VkRect2D &renderArea/*, const ImGuiData &imGuiData*/,
                                            const std::shared_ptr<Pipeline> &compositePipeline,
                                            const std::shared_ptr<DescriptorSet> &compositeDescriptorSet) {
    VkRenderPassBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = renderPass,
            .framebuffer = framebuffer,
            .renderArea = renderArea,
            .clearValueCount = static_cast<uint32_t>(std::size(clearValues)),
            .pClearValues = clearValues,
    };

    VkCommandBufferBeginInfo cmdBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
    };

    VkCommandBufferAllocateInfo allocateInfo{
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

    bool bound = false;

    VkPipeline currentPipeline = VK_NULL_HANDLE;
    for (auto &drawCall: drawData.getDrawCalls()) {
        PushData pushData{
                .model = drawCall.model,
                .view = drawData.view,
                .projection = drawData.projection,
                .viewPosition = drawData.position,
        };

        VkPipeline pipeline = drawCall.pipeline;
        if (currentPipeline != pipeline) {
            currentPipeline = drawCall.pipeline;
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall.pipeline);
        }

        vkCmdPushConstants(cmd, drawCall.pipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushData),
                           &pushData);

        vkCmdBindVertexBuffers(cmd, 0, 4, drawCall.buffers.data(), drawCall.offsets.data());

        if (!bound) {
            bound = true;
            vkCmdBindIndexBuffer(cmd, drawCall.indexBuffer, 0,
                                 VK_INDEX_TYPE_UINT16);
        }

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, drawCall.pipelineLayout, 0, 1,
                                &drawCall.descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, drawCall.indexCount, 1, drawCall.firstIndex, 0, 0);
    }

    vkCmdNextSubpass(cmd, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, compositePipeline->getPipeline());
    VkDescriptorSet set = compositeDescriptorSet->getDescriptorSet();
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, compositePipeline->getPipelineLayout(), 0, 1,
                            &set, 0, nullptr);
    vkCmdDraw(cmd, 3, 1, 0, 0);

    //ImGuiData::appendDrawToCommandBuffer(cmd);

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

    return cmd;
}
