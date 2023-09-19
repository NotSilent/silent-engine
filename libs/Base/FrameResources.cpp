#include "FrameResources.h"

#include "Image.h"
#include "VkInit.h"
#include <limits>

FrameResources::FrameResources(VkDevice device,
                               VmaAllocator allocator,
                               VkImage swapchainImage,
                               VkImageView swapchainImageView,
                               const VkRect2D &renderArea)
        : device(device)
        , frameFence(VkInit::createFence(device, VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT))
        , _swapchainImage(swapchainImage)
        , _swapchainImageView(swapchainImageView) {
    ImageCreateInfo colorImageCreateInfo{
            .extent = {renderArea.extent.width, renderArea.extent.height, 1},
            .imageType = VK_IMAGE_TYPE_2D,
            .format = VK_FORMAT_R16G16B16A16_SFLOAT,
            .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    };

    _colorImage = std::make_shared<Image>(device, allocator, colorImageCreateInfo);
}

FrameResources::FrameResources(FrameResources &&other) noexcept {
    device = other.device;
    frameFence = other.frameFence;

    _swapchainImage = other._swapchainImage;
    _swapchainImageView = other._swapchainImageView;
    // TODO: Move?
    _colorImage = other._colorImage;
}

FrameResources &FrameResources::operator=(FrameResources &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    device = other.device;
    frameFence = other.frameFence;

    _swapchainImage = other._swapchainImage;
    _swapchainImageView = other._swapchainImageView;
    // TODO: Move?
    _colorImage = other._colorImage;

    return *this;
}

void FrameResources::destroy(VkDevice device, VmaAllocator allocator) {
    _colorImage->destroy(device, allocator);
}

VkImage FrameResources::getSwapchainImage() const {
    return _swapchainImage;
}

VkImageView FrameResources::getSwapchainImageView() const {
    return _swapchainImageView;
}

void FrameResources::waitFence() {
    vkWaitForFences(device, 1, &frameFence, true, std::numeric_limits<uint64_t>::max());
    vkResetFences(device, 1, &frameFence);
}

VkFence FrameResources::getFrameFence() {
    return frameFence;
}
