#pragma once
#include <vulkan/vulkan.h>

#include "vk-bootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

namespace VkInit {
//TODO: Format and usage as parameters
//VkImage createDepthStencilImage(const vkb::Device& device, uint32_t width, uint32_t height)
//{
//    VkImageCreateInfo createInfo {
//        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
//        .pNext = nullptr,
//        .flags = {},
//        .imageType = VK_IMAGE_TYPE_2D,
//        .format = VK_FORMAT_D24_UNORM_S8_UINT,
//        .extent = { width, height, 1 },
//        .mipLevels = 1,
//        .arrayLayers = 1,
//        .samples = VK_SAMPLE_COUNT_1_BIT,
//        .tiling = VK_IMAGE_TILING_OPTIMAL,
//        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
//        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
//        .queueFamilyIndexCount = 0,
//        .pQueueFamilyIndices = nullptr,
//        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
//    };
//
//    VkImage image;
//    if (vkCreateImage(device.device, &createInfo, nullptr, &image) != VK_SUCCESS) {
//        throw std::runtime_error("Error: vkCreateImage");
//    }
//
//    return image;
//}
//
////TODO: Format and usage as parameters
//VkImageView createDepthStencilImageView(const vkb::Device& device, const VkImage image)
//{
//    VkImageViewCreateInfo createInfo {
//        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
//        .pNext = nullptr,
//        .flags = {},
//        .image = image,
//        .viewType = VK_IMAGE_VIEW_TYPE_2D,
//        .format = VK_FORMAT_D24_UNORM_S8_UINT,
//        .components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
//        .subresourceRange = {
//            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
//            .baseMipLevel = 0,
//            .levelCount = 1,
//            .baseArrayLayer = 0,
//            .layerCount = 1,
//        },
//    };
//
//    VkImageView imageView;
//    if (vkCreateImageView(device.device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
//        throw std::runtime_error("Error: vkCreateImageView");
//    }
//
//    return imageView;
//}

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
        .flags = {},
    };

    VkFence fence;
    if (vkCreateFence(device.device, &createInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create framebuffer");
    }

    return fence;
}

VkFramebuffer createFramebuffer(const vkb::Device& device, const VkRenderPass renderPass, const VkImageView color, VkImageView depthStencil, const uint32_t width, const uint32_t height)
{
    VkImageView attachments[] { color, depthStencil };

    VkFramebufferCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .renderPass = renderPass,
        .attachmentCount = 2,
        .pAttachments = attachments,
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
    VkAttachmentReference colorAttachment {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference depthStencilAttachment {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpassDescription {
        .flags = {},
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = &depthStencilAttachment,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    VkAttachmentDescription attachmentDescriptions[] {
        {
            .flags = {},
            .format = swapchain.image_format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        },
        {
            .flags = {},
            .format = VK_FORMAT_D24_UNORM_S8_UINT,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL ,
        },
    };

    VkRenderPassCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .attachmentCount = 2,
        .pAttachments = attachmentDescriptions,
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
