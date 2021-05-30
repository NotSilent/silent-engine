#pragma once
#include <vulkan/vulkan.h>


#include "ImGuiData.h"
#include "Mesh.h"
#include "vk-bootstrap/VkBootstrap.h"

// TODO: Decouple ImGui

namespace VkDraw {
VkCommandBuffer recordCommandBuffer(const vkb::Device& device, VkCommandPool commandPool, const Mesh* mesh, const VkRenderPass renderPass,
    const VkFramebuffer framebuffer, const VkRect2D& renderArea, uint32_t clearValueCount, VkClearValue* clearValues, const ImGuiData* imGuiData, const PushData& pushData)
{
    VkRenderPassBeginInfo beginInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = renderPass,
        .framebuffer = framebuffer,
        .renderArea = renderArea,
        .clearValueCount = 1,
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

    VkDeviceSize offset { 0 };
    VkBuffer buffer = mesh->getVertexBuffer();
    VkPipelineLayout pipelineLayout = mesh->getPipelineLayout();
    VkPipeline pipeline = mesh->getPipeline();
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    vkCmdBindVertexBuffers(cmd, 0, 1, &buffer, &offset);
    vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT , 0, sizeof(PushData), &pushData);

    vkCmdDraw(cmd, mesh->getVertexCount(), 1, 0, 0);

    imGuiData->appendDrawToCommandBuffer(cmd);

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);

    

    return cmd;
}
}
