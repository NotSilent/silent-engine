#include "VkInit.h"

VmaAllocator VkInit::createAllocator(vk::Instance instance, vk::PhysicalDevice physicalDevice,
                                     vk::Device device, uint32_t vulkanApiVersion) {

    VmaAllocatorCreateInfo createInfo{
            .flags = {},
            .physicalDevice = physicalDevice,
            .device = device,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pDeviceMemoryCallbacks = nullptr,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = nullptr,
            .instance = instance,
            .vulkanApiVersion = vulkanApiVersion,
    };

    VmaAllocator allocator;
    if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS) {
        throw std::runtime_error("vmaCreateAllocator");
    }

    return allocator;
}

vk::Semaphore VkInit::createSemaphore(vk::Device device) {
    vk::SemaphoreCreateInfo createInfo;

    vk::Semaphore semaphore;
    if (device.createSemaphore(&createInfo, nullptr, &semaphore) != vk::Result::eSuccess) {
        throw std::runtime_error("Couldn't create semaphore");
    }

    return semaphore;
}

vk::Fence VkInit::createFence(vk::Device device, vk::FenceCreateFlags flags) {
    vk::FenceCreateInfo createInfo{flags};

    vk::Fence fence;
    if (device.createFence(&createInfo, nullptr, &fence) != vk::Result::eSuccess) {
        throw std::runtime_error("Couldn't create fence");
    }

    return fence;
}

vk::CommandPool VkInit::createCommandPool(vk::Device device, uint32_t queueFamilyIndex) {
    vk::CommandPoolCreateInfo createInfo{vk::CommandPoolCreateFlagBits::eTransient, queueFamilyIndex};

    vk::CommandPool commandPool;
    if (device.createCommandPool(&createInfo, nullptr, &commandPool) != vk::Result::eSuccess) {
        throw std::runtime_error("Couldn't create command pool");
    }

    return commandPool;
}