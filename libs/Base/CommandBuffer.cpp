#include "CommandBuffer.h"

#include <vector>

void CommandBuffer::pipelineBarrier(VkCommandBuffer cmd, VkPipelineStageFlags srcStageMask,
                                    VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask,
                                    VkAccessFlags dstAccessMask, VkImageLayout oldLayout, VkImageLayout newLayout,
                                    uint32_t srcQueueFamilyIndex, uint32_t dstQueueFamilyIndex, VkImage image,
                                    VkImageAspectFlags aspectMask) {
    VkImageSubresourceRange subresourceRange{
            .aspectMask = aspectMask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
    };
    std::vector<VkImageMemoryBarrier> memoryBarriers;
    memoryBarriers.push_back(VkImageMemoryBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = srcAccessMask,
            .dstAccessMask = dstAccessMask,
            .oldLayout = oldLayout,
            .newLayout = newLayout,
            .srcQueueFamilyIndex = srcQueueFamilyIndex,
            .dstQueueFamilyIndex = dstQueueFamilyIndex,
            .image = image,
            .subresourceRange = subresourceRange,
    });
    vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, {}, 0, nullptr, 0, nullptr, memoryBarriers.size(),
                         memoryBarriers.data());
}
