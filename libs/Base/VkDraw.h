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
    void recordCommandBuffer(VkCommandBuffer cmd, const DrawData &drawData,
                                        VkImage swapchainImage,
                                        VkImageView swapchainImageView,
                                        uint32_t graphicsFamilyIndex,
                                        const VkRect2D &renderArea);
}
