#pragma once

#include <memory>
#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

class Image;

class FrameResources {
private:
    VkImage _swapchainImage;
    VkImageView _swapchainImageView;

    // Should be just a handle, from ImageManager
    std::shared_ptr<Image> _colorImage;

public:
    FrameResources(VkDevice device,
                   VmaAllocator allocator,
                   VkImage swapchainImage,
                   VkImageView swapchainImageView,
                   const VkRect2D &renderArea);

    ~FrameResources() = default;

    FrameResources(const FrameResources &other) = delete;

    FrameResources &operator=(const FrameResources &other) = delete;

    FrameResources(FrameResources &&other) noexcept;

    FrameResources &operator=(FrameResources &&other) noexcept;

    // Should be handled by whatever will create Images
    void destroy(VkDevice device, VmaAllocator allocator);

    [[nodiscard]] VkImage getSwapchainImage() const;

    [[nodiscard]] VkImage getSwapchainImageView() const;
};
