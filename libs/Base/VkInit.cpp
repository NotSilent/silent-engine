#include "VkInit.h"

VmaAllocator VkInit::createAllocator(const vkb::Instance &instance, const vkb::PhysicalDevice &physicalDevice,
                                     const vkb::Device &device, uint32_t vulkanApiVersion,
                                     const uint32_t frameInUseCount) {
    VmaRecordSettings recordSettings{
            .flags = {},
            .pFilePath = "vma_log.csv",
    };

    VmaAllocatorCreateInfo createInfo{
            .flags = {},
            .physicalDevice = physicalDevice.physical_device,
            .device = device.device,
            .preferredLargeHeapBlockSize = 0,
            .pAllocationCallbacks = nullptr,
            .pDeviceMemoryCallbacks = nullptr,
            .frameInUseCount = frameInUseCount,
            .pHeapSizeLimit = nullptr,
            .pVulkanFunctions = nullptr,
            .pRecordSettings = &recordSettings,
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
        throw std::runtime_error("Couldn't create framebuffer");
    }

    return fence;
}

VkFramebuffer
VkInit::createFramebuffer(const vkb::Device &device, const VkRenderPass renderPass, const VkImageView color,
                          VkImageView depthStencil, const uint32_t width, const uint32_t height) {
    VkImageView attachments[]{color, depthStencil};

    VkFramebufferCreateInfo createInfo{
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

VkRenderPass VkInit::createRenderPass(const vkb::Device &device, const vkb::Swapchain &swapchain) {
    VkAttachmentReference colorAttachment{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference depthStencilAttachment{
            .attachment = 1,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    VkSubpassDescription subpassDescription{
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

    VkAttachmentDescription attachmentDescriptions[]{
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
                    .format = VK_FORMAT_D32_SFLOAT_S8_UINT,
                    .samples = VK_SAMPLE_COUNT_1_BIT,
                    .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                    .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            },
    };

    VkRenderPassCreateInfo createInfo{
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
