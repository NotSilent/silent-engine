#pragma once
#include <memory>
#include <vulkan/vulkan.h>

#include "ImGuiData.h"
#include "Mesh.h"
#include "Texture.h"
#include "vk-bootstrap/VkBootstrap.h"
#include <DrawData.h>

// TODO: Decouple ImGui

namespace VkDraw {
VkCommandBuffer recordCommandBuffer(vkb::Device& device, VkCommandPool commandPool, const DrawData& drawData, VkRenderPass renderPass,
    VkFramebuffer framebuffer, const VkRect2D& renderArea, const ImGuiData& imGuiData);
}
