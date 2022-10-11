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
        throw std::runtime_error("Couldn't create framebuffer");
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

VkRenderPass VkInit::createRenderPass(VkDevice device, VkFormat swapchainImageFormat) {
    // Abstraction of top of image?
    std::array<VkAttachmentDescription, 5> attachmentDescriptions{
            {
                    {
                            // swapchain color
                            .flags = {},
                            .format = swapchainImageFormat,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                    },
                    {
                            // color
                            .flags = {},
                            .format = VK_FORMAT_R8G8B8A8_UNORM,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    {
                            // position
                            .flags = {},
                            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    {
                            // normal
                            .flags = {},
                            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
                            .samples = VK_SAMPLE_COUNT_1_BIT,
                            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                            .finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    {
                            // depth
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
            }
    };

    std::array<VkAttachmentReference, 3> offscreenAttachments{
            {
                    {
                            // color
                            .attachment = 1,
                            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    },
                    {
                            // position
                            .attachment = 2,
                            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    },
                    {
                            // normal
                            .attachment = 3,
                            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                    }
            }
    };

    std::array<VkAttachmentReference, 3> compositeAttachments{
            {
                    {
                            // color
                            .attachment = 1,
                            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    {
                            // position
                            .attachment = 2,
                            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    },
                    {
                            // normal
                            .attachment = 3,
                            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                    }
            }
    };

    VkAttachmentReference compositeAttachement{
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentReference depthAttachment{
            .attachment = 4,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    };

    std::array<VkSubpassDescription, 2> subpassDescriptions{
            {
                    {
                            .flags = {},
                            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                            .inputAttachmentCount = 0,
                            .pInputAttachments = nullptr,
                            .colorAttachmentCount = offscreenAttachments.size(),
                            .pColorAttachments = offscreenAttachments.data(),
                            .pResolveAttachments = nullptr,
                            .pDepthStencilAttachment = &depthAttachment,
                            .preserveAttachmentCount = 0,
                            .pPreserveAttachments = nullptr,
                    },
                    {
                            .flags = {},
                            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
                            .inputAttachmentCount = compositeAttachments.size(),
                            .pInputAttachments = compositeAttachments.data(),
                            .colorAttachmentCount = 1,
                            .pColorAttachments = &compositeAttachement,
                            .pResolveAttachments = nullptr,
                            .pDepthStencilAttachment = nullptr,
                            .preserveAttachmentCount = 0,
                            .pPreserveAttachments = nullptr,
                    }
            }
    };
    std::array<VkSubpassDependency, 1> dependencies{};
    dependencies[0].srcSubpass = 0;
    dependencies[0].dstSubpass = 1;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .attachmentCount = attachmentDescriptions.size(),
            .pAttachments = attachmentDescriptions.data(),
            .subpassCount = 2,
            .pSubpasses = subpassDescriptions.data(),
            .dependencyCount = dependencies.size(),
            .pDependencies = dependencies.data(),
    };

    VkRenderPass renderPass;
    if (vkCreateRenderPass(device, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
        throw std::runtime_error("Couldn't create renderPass");
    }

    return renderPass;
}