#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>
#include "Mesh.h"
#include "Texture.h"
#include "VkBootstrap.h"
#include "DrawData.h"
#include "Pipeline.h"

class DescriptorSet;

namespace VkDraw {
    VkCommandBuffer recordCommandBuffer(vkb::Device &device, VkCommandPool commandPool, const DrawData &drawData,
                                        VkImage swapchainImage,
                                        VkImageView swapchainImageView,
                                        uint32_t graphicsFamilyIndex,
                                        const VkRect2D &renderArea);
}
