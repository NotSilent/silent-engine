#pragma once
#include <vulkan/vulkan.h>

#include "vk-bootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

namespace VkInit {
VmaAllocator createAllocator(const vkb::Instance& instance, const vkb::PhysicalDevice& physicalDevice, const vkb::Device& device, uint32_t vulkanApiVersion, const uint32_t frameInUseCount)
{
    VmaAllocatorCreateInfo createInfo {
        .flags = {},
        .physicalDevice = physicalDevice.physical_device,
        .device = device.device,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .frameInUseCount = frameInUseCount,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = nullptr,
        .pRecordSettings = nullptr,
        .instance = instance.instance,
        .vulkanApiVersion = vulkanApiVersion,
    };

    VmaAllocator allocator;
    if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create semaphore");
    }

    return allocator;
}

VkDescriptorPool createDescriptorPool(const vkb::Device& device)
{
    VkDescriptorPoolSize descriptorPoolSize {
        .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
    };

    VkDescriptorPoolCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .maxSets = 1,
        .poolSizeCount = 1,
        .pPoolSizes = &descriptorPoolSize,
    };

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(device.device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateDescriptorPool");
    }

    return descriptorPool;
}

VkDescriptorSetLayout createDefaultDescriptorSetLayout(const vkb::Device& device)
{
    VkDescriptorSetLayoutBinding layoutBinding {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr,
    };

    VkDescriptorSetLayoutCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .bindingCount = 1,
        .pBindings = &layoutBinding,
    };

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device.device, &createInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateDescriptorSetLayout");
    }

    return descriptorSetLayout;
}

VkSemaphore createSemaphore(const vkb::Device& device)
{
    VkSemaphoreCreateInfo createInfo {
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

VkFence createFence(const vkb::Device& device)
{
    VkFenceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = /*VK_FENCE_CREATE_SIGNALED_BIT*/ {},
    };

    VkFence fence;
    if (vkCreateFence(device.device, &createInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create framebuffer");
    }

    return fence;
}

VkFramebuffer createFramebuffer(const vkb::Device& device, const VkRenderPass renderPass, const VkImageView imageView, const uint32_t width, const uint32_t height)
{
    VkFramebufferCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .renderPass = renderPass,
        .attachmentCount = 1,
        .pAttachments = &imageView,
        .width = width,
        .height = height,
        .layers = 1,
    };

    VkFramebuffer framebuffer;
    if (vkCreateFramebuffer(device.device, &createInfo, nullptr, &framebuffer) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create framebuffer");
    }

    return framebuffer;
}

VkCommandPool createCommandPool(const vkb::Device& device)
{
    VkCommandPoolCreateInfo createInfo {
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

VkRenderPass createRenderPass(const vkb::Device& device, const vkb::Swapchain& swapchain)
{
    VkAttachmentReference attachmentReference {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpassDescription {
        .flags = {},
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    VkAttachmentDescription attachmentDescription {
        .flags = {},
        .format = swapchain.image_format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    VkRenderPassCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .attachmentCount = 1,
        .pAttachments = &attachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 0,
        .pDependencies = nullptr,
    };

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device.device, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create renderPass");
    }

    return renderPass;
}
}
