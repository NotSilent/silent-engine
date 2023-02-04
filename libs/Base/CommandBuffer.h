#pragma once

#include "vulkan/vulkan_core.h"

class CommandBuffer {
private:

public:
    // TODO: All statics to members
    static void pipelineBarrier(VkCommandBuffer cmd, VkPipelineStageFlags srcStageMask,
                                VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask,
                                VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout,
                                uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex, VkImage image,
                                VkImageAspectFlags aspectFlags);
};
