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

    std::vector<VkBuffer> vertexBuffers;
    std::vector<VkDeviceSize> offsets;
    auto view = drawData.getCamera()->getViewMatrix();
    auto projection = drawData.getCamera()->getProjectionMatrix();
    auto viewPosition = drawData.getCamera()->getPosition();

    VkPipeline currentPipeline = VK_NULL_HANDLE;
    for (auto &drawCall: drawData.getDrawCalls()) {
        vertexBuffers.clear();
        offsets.clear();

        PushData pushData{
                .model = drawCall.model,
                .view = view,
                .projection = projection,
                .viewPosition = viewPosition,
        };

        for (auto &attribute: drawCall.mesh->getAttributes()) {
            vertexBuffers.push_back(attribute.buffer->getBuffer());
            offsets.push_back(0);
        }

        std::shared_ptr<Material> material = drawCall.material;
        VkDescriptorSet descriptorSet = material->getDescriptorSet();

        VkPipeline pipeline = material->getPipeline();
        if (currentPipeline != pipeline) {
            currentPipeline = material->getPipeline();
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->getPipeline());
        }

        vkCmdPushConstants(cmd, material->getPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushData), &pushData);
        vkCmdBindVertexBuffers(cmd, 0, static_cast<uint32_t>(vertexBuffers.size()), vertexBuffers.data(),
                               offsets.data());
        vkCmdBindIndexBuffer(cmd, drawCall.mesh->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->getPipelineLayout(), 0, 1,
                                &descriptorSet, 0, nullptr);

        vkCmdDrawIndexed(cmd, drawCall.mesh->getIndexCount(), 1, 0, 0, 0);
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
