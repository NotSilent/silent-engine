#pragma once

#include <vulkan/vulkan_core.h>

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

namespace VkInit {
    VmaAllocator
    createAllocator(const vkb::Instance &instance, const vkb::PhysicalDevice &physicalDevice, const vkb::Device &device,
                    uint32_t vulkanApiVersion);

    VkSemaphore createSemaphore(VkDevice device);

    VkFence createFence(VkDevice device, VkFenceCreateFlags flags);

    VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamilyIndex);
}
