#pragma once

#include <vulkan/vulkan.hpp>

struct RenderPassAttachmentOutput {
    vk::Image image;
    vk::ImageView imageView;
    vk::ImageLayout imageLayout = vk::ImageLayout::eUndefined;
};
