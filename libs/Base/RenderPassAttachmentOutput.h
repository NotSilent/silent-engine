#pragma once
#include <vulkan/vulkan_core.h>

struct RenderPassAttachmentOutput {
    VkImage image;
    VkImageView imageView;
    VkImageLayout imageLayout;
};
