#include <array>
#include "VkInit.h"

VmaAllocator VkInit::createAllocator(const vkb::Instance &instance, const vkb::PhysicalDevice &physicalDevice,
                                     const vkb::Device &device, uint32_t vulkanApiVersion) {

    VmaAllocatorCreateInfo createInfo{
            .flags = {},
            .physicalDevice = physicalDevice.physical_device,
            .device = device.device,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = nullptr,
            .instance = instance.instance,
            .vulkanApiVersion = vulkanApiVersion,
    };

    VmaAllocator allocator;
    if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS) {
        throw std::runtime_error("vmaCreateAllocator");
    }

    return allocator;
}

VkSemaphore VkInit::createSemaphore(const vkb::Device &device) {
    VkSemaphoreCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {}
    };

    VkSemaphore semaphore;
    if (vkCreateSemaphore(device.device, &createInfo, nullptr, &semaphore) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create semaphore");
    }

    return semaphore;
}

VkFence VkInit::createFence(const vkb::Device &device, VkFenceCreateFlags flags) {
    VkFenceCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = flags,
    };

    VkFence fence;
    if (vkCreateFence(device.device, &createInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create fence");
    }

    return fence;
}

VkCommandPool VkInit::createCommandPool(const vkb::Device &device) {
    VkCommandPoolCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = device.get_queue_index(vkb::QueueType::graphics).value(),
    };

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device.device, &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create command pool");
    }

    return commandPool;
}