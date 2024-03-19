#pragma once

#include <vulkan/vulkan.hpp>

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

namespace VkInit {
    VmaAllocator
    createAllocator(vk::Instance instance, vk::PhysicalDevice physicalDevice, vk::Device device,
                    uint32_t vulkanApiVersion);

    vk::Semaphore createSemaphore(vk::Device device);

    vk::Fence createFence(vk::Device device, vk::FenceCreateFlags flags);

    vk::CommandPool createCommandPool(vk::Device device, uint32_t queueFamilyIndex);
}
