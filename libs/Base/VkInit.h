#pragma once

#include <vulkan/vulkan_core.h>

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

namespace VkInit {
    VmaAllocator
    createAllocator(const vkb::Instance &instance, const vkb::PhysicalDevice &physicalDevice, const vkb::Device &device,
                    uint32_t vulkanApiVersion);

    VkSemaphore createSemaphore(const vkb::Device &device);

    VkFence createFence(const vkb::Device &device, VkFenceCreateFlags flags);

    VkCommandPool createCommandPool(const vkb::Device &device);
}
