#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "ImGuiData.h"
#include "Mesh.h"
#include "Texture.h"
#include "VkBootstrap.h"
#include "DrawData.h"
#include "Pipeline.h"

// TODO: Decouple ImGui

class DescriptorSet;

namespace VkDraw {
    VkCommandBuffer recordCommandBuffer(vkb::Device &device, VkCommandPool commandPool, const DrawData &drawData,
                                        VkRenderPass renderPass,
                                        VkFramebuffer framebuffer, const VkRect2D &renderArea,
                                        /*const ImGuiData &imGuiData, */const std::shared_ptr<Pipeline> &compositePipeline,
                                        const std::shared_ptr<DescriptorSet> &compositeDescriptorSet);
}
