#include "Image.h"

Image::Image(const vkb::Device &device, VmaAllocator allocator, VkCommandPool commandPool, uint32_t width,
             uint32_t height, VkFormat format, uint32_t size, const void *data)
        : _image{}, _allocation{} {
    VkExtent3D extent{width, height, 1};

    VkBufferCreateInfo bufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .size = size,
            .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
    };

    VmaAllocationCreateInfo allocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_CPU_ONLY,
            .requiredFlags = {},
            .preferredFlags = {},
            .memoryTypeBits = {},
            .pool = nullptr,
            .pUserData = nullptr,
    };

    VkBuffer stagingBuffer;
    VmaAllocation stagingBufferAlloc;
    VmaAllocationInfo stagingBufferAllocInfo;
    if (vmaCreateBuffer(allocator, &bufferCreateInfo, &allocationCreateInfo, &stagingBuffer, &stagingBufferAlloc,
                        &stagingBufferAllocInfo) != VK_SUCCESS) {
        throw std::runtime_error("Error: vmaCreateImage");
    }

    memcpy(stagingBufferAllocInfo.pMappedData, data, size);

    // No need to flush stagingVertexBuffer memory because CPU_ONLY memory is always HOST_COHERENT.

    VkImageCreateInfo imageCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = format,
            .extent = extent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.flags = 0;

    if (vmaCreateImage(allocator, &imageCreateInfo, &allocationCreateInfo, &_image, &_allocation, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("Error: vmaCreateImage");
    }

    VkCommandBufferAllocateInfo commandBufferInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
    };

    VkCommandBuffer transferCommandBuffer;
    if (vkAllocateCommandBuffers(device.device, &commandBufferInfo, &transferCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkAllocateCommandBuffers");
    }

    VkCommandBufferBeginInfo transferCmdBeginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
            .pInheritanceInfo = nullptr,
    };

    if (vkBeginCommandBuffer(transferCommandBuffer, &transferCmdBeginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkBeginCommandBuffer");
    }

    VkImageSubresourceRange range{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
    };

    VkImageMemoryBarrier imageBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = VK_ACCESS_NONE_KHR,
            .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = _image,
            .subresourceRange = range,
    };

    vkCmdPipelineBarrier(transferCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         {}, 0, nullptr, 0, nullptr, 1, &imageBarrier);

    VkImageSubresourceLayers layer{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
    };

    VkBufferImageCopy bufferImageCopy{
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource = layer,
            .imageOffset = {0, 0},
            .imageExtent = extent,
    };

    vkCmdCopyBufferToImage(transferCommandBuffer, stagingBuffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &bufferImageCopy);

    imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    imageBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(transferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);

    if (vkEndCommandBuffer(transferCommandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkEndCommandBuffer");
    }

    VkSubmitInfo submitTransferInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &transferCommandBuffer,
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
    };

    if (vkQueueSubmit(device.get_queue(vkb::QueueType::graphics).value(), 1, &submitTransferInfo, VK_NULL_HANDLE) !=
        VK_SUCCESS) {
        throw std::runtime_error("Error: vkQueueSubmit");
    }
    if (vkQueueWaitIdle(device.get_queue(vkb::QueueType::graphics).value()) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkQueueWaitIdle");
    }

    vkFreeCommandBuffers(device.device, commandPool, 1, &transferCommandBuffer);
    vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferAlloc);

    VkImageViewCreateInfo viewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .image = _image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = format,
            .components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                           VK_COMPONENT_SWIZZLE_A},
            .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
            },
    };

    if (vkCreateImageView(device.device, &viewCreateInfo, nullptr, &_imageView) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateImageView");
    }
}

Image::Image(VkDevice device, VmaAllocator allocator, const ImageCreateInfo &imageCreateInfo) {
    VkImageCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .imageType = imageCreateInfo.imageType,
            .format = imageCreateInfo.format,
            .extent = imageCreateInfo.extent,
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = imageCreateInfo.usage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo allocationCreateInfo{
            .flags = {},
            .usage = VMA_MEMORY_USAGE_GPU_ONLY,
            .requiredFlags = {},
            .preferredFlags = {},
            .memoryTypeBits = {},
            .pool = {},
            .pUserData = nullptr,
    };

    if (vmaCreateImage(allocator, &createInfo, &allocationCreateInfo, &_image, &_allocation, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("Error: vmaCreateImage");
    }

    VkImageViewCreateInfo viewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .image = _image,
            .viewType = imageCreateInfo.viewType,
            .format = imageCreateInfo.format,
            .components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B,
                           VK_COMPONENT_SWIZZLE_A},
            .subresourceRange = {
                    .aspectMask = imageCreateInfo.aspectMask,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
            },
    };

    if (vkCreateImageView(device, &viewCreateInfo, nullptr, &_imageView) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateImageView");
    }
}

void Image::destroy(VkDevice device, VmaAllocator allocator) {
    vkDestroyImageView(device, _imageView, nullptr);
    vmaDestroyImage(allocator, _image, _allocation);
}

VkImageView Image::getImageView() const {
    return _imageView;
}

VkImage Image::getImage() const {
    return _image;
}
