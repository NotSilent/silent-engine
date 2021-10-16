#pragma once
#include <memory>
#include <vulkan/vulkan.h>

#include "ImGuiData.h"
#include "Mesh.h"
#include "Texture.h"
#include "vk-bootstrap/VkBootstrap.h"

// TODO: Decouple ImGui

namespace VkDraw {
VkCommandBuffer recordCommandBuffer(VkDevice device, VkCommandPool commandPool, const std::vector<std::shared_ptr<Mesh>> meshes, VkPipelineLayout pipelineLayout, VkPipeline pipeline, VkRenderPass renderPass,
    VkFramebuffer framebuffer, const VkRect2D& renderArea, const ImGuiData& imGuiData, const PushData& pushData, VkDescriptorSet descriptorSet);
}
