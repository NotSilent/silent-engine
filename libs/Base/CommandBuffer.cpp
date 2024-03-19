#include "CommandBuffer.h"

#include <vector>

void CommandBuffer::pipelineBarrier(vk::CommandBuffer cmd, vk::PipelineStageFlags srcStageMask,
                                    vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask,
                                    vk::AccessFlags dstAccessMask, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                                    vk::Image image,
                                    vk::ImageAspectFlags aspectMask) {
    vk::ImageSubresourceRange subresourceRange(aspectMask, 0, 1, 0, 1);

    std::vector<vk::ImageMemoryBarrier> memoryBarriers;
    memoryBarriers.emplace_back(
            srcAccessMask,
            dstAccessMask,
            oldLayout,
            newLayout,
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            image,
            subresourceRange);

    cmd.pipelineBarrier(srcStageMask,
                        dstStageMask,
                        {},
                        0,
                        nullptr,
                        0,
                        nullptr,
                        memoryBarriers.size(),
                        memoryBarriers.data());
}
