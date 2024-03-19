#pragma once

#include <stdexcept>

#include <vulkan/vulkan.hpp>

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

#include "stb_image.h"

struct ImageCreateInfo {
    vk::Extent3D extent;

    vk::ImageType imageType;
    vk::Format format;
    vk::ImageUsageFlags usage;

    vk::ImageViewType viewType;
    vk::ImageAspectFlags aspectMask;
};

// TODO: Move creation to image manager
// Allocation stored with ImageAllocation{Image, VmaAllocation}

class Image {
private:
    vk::Image _image = nullptr;
    vk::ImageView _imageView = nullptr;

    VmaAllocation _allocation = nullptr;

public:
    Image() = default;

    Image(vk::Image image, vk::ImageView imageView);

    Image(vk::Device &device, vk::Queue graphicsQueue, VmaAllocator allocator, vk::CommandPool commandPool, uint32_t width, uint32_t height,
          vk::Format format, uint32_t size, const void *data);

    Image(vk::Device device, VmaAllocator allocator, const ImageCreateInfo &imageCreateInfo);

    Image(const Image &other) = delete;

    Image &operator=(const Image &other) = delete;

    Image(Image &&other) = default;

    Image &operator=(Image &&other) = default;

    void destroy(vk::Device device, VmaAllocator allocator);

    [[nodiscard]] vk::ImageView getImageView() const;

    [[nodiscard]] vk::Image getImage() const;
};