#pragma once

#include <stdexcept>

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

#include "stb_image.h"

struct ImageCreateInfo {
    VkExtent3D extent;

    VkImageType imageType;
    VkFormat format;
    VkImageUsageFlags usage;

    VkImageViewType viewType;
    VkImageAspectFlags aspectMask;
};

class Image {
private:
    VkImage _image = VK_NULL_HANDLE;
    VkImageView _imageView = VK_NULL_HANDLE;

    VmaAllocation _allocation = VK_NULL_HANDLE;

public:
    Image() = default;

    Image(const vkb::Device &device, VmaAllocator allocator, VkCommandPool commandPool, uint32_t width, uint32_t height,
          VkFormat format, uint32_t size, const void *data);

    Image(VkDevice device, VmaAllocator allocator, const ImageCreateInfo &imageCreateInfo);

    Image(const Image &other) = delete;

    Image &operator=(const Image &other) = delete;

    Image(Image &&other) = default;

    Image &operator=(Image &&other) = default;

    void destroy(VkDevice device, VmaAllocator allocator);

    [[nodiscard]] VkImageView getImageView() const;

    [[nodiscard]] VkImage getImage() const;
};