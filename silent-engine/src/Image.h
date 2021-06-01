#pragma once
#include <stdexcept>

#include "vk-bootstrap/VkBootstrap.h"
#include "vma/vk_mem_alloc.h"

class Image {
public:
private:
    vkb::Device _device;
    VmaAllocator _allocator;

    VkImage _image;
    VkImageView _imageView;
    VmaAllocation _allocation;

public:
    Image(const vkb::Device& device, const VmaAllocator allocator, uint32_t width, uint32_t height)
    {
        _device = device;
        _allocator = allocator;

        VkImageCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_D24_UNORM_S8_UINT,
            .extent = { width, height, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        VmaAllocationCreateInfo allocationCreateInfo {
            .flags = {},
            .usage = VMA_MEMORY_USAGE_GPU_ONLY,
            .requiredFlags = {},
            .preferredFlags = {},
            .memoryTypeBits = {},
            .pool = {},
            .pUserData = nullptr,
        };

        if (vmaCreateImage(_allocator, &createInfo, &allocationCreateInfo, &_image, &_allocation, nullptr) != VK_SUCCESS) {
            throw std::runtime_error("Error: vmaCreateImage");
        }

        VkImageViewCreateInfo viewCreateInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .image = _image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = VK_FORMAT_D24_UNORM_S8_UINT,
            .components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT,
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

    ~Image()
    {
        vkDestroyImageView(_device.device, _imageView, nullptr);
        vmaDestroyImage(_allocator, _image, _allocation);
    }

    VkImageView getImageView() const
    {
        return _imageView;
    }
};