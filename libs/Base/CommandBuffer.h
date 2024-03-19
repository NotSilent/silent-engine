#pragma once

#include "vulkan/vulkan.hpp"

class CommandBuffer {
private:

public:
    // TODO: All statics to members
    static void pipelineBarrier(vk::CommandBuffer cmd, vk::PipelineStageFlags srcStageMask,
                                vk::PipelineStageFlags dstStageMask, vk::AccessFlags srcAccessMask,
                                vk::AccessFlags dstAccessMask, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::Image image,
                                vk::ImageAspectFlags aspectFlags);
};
