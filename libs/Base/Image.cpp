#include "Image.h"

Image::Image(vk::Device &device, vk::Queue graphicsQueue, VmaAllocator allocator, vk::CommandPool commandPool,
             uint32_t width,
             uint32_t height, vk::Format format, uint32_t size, const void *data)
        : _image{}, _allocation{} {
    vk::Extent3D extent{width, height, 1};

    vk::BufferCreateInfo bufferCreateInfo(
            {},
            size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::SharingMode::eExclusive,
            0,
            nullptr
    );

    VmaAllocationCreateInfo allocationCreateInfo{
            .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
            .usage = VMA_MEMORY_USAGE_CPU_ONLY,
            .requiredFlags = {},
            .preferredFlags = {},
            .memoryTypeBits = {},
            .pool = nullptr,
            .pUserData = nullptr,
    };

    vk::Buffer stagingBuffer;
    VmaAllocation stagingBufferAlloc;
    VmaAllocationInfo stagingBufferAllocInfo;
    if (vmaCreateBuffer(allocator, reinterpret_cast<const VkBufferCreateInfo *>(&bufferCreateInfo),
                        &allocationCreateInfo,
                        reinterpret_cast<VkBuffer *>(&stagingBuffer), &stagingBufferAlloc,
                        &stagingBufferAllocInfo) != VK_SUCCESS) {
        throw std::runtime_error("Error: vmaCreateImage");
    }

    memcpy(stagingBufferAllocInfo.pMappedData, data, size);

    // No need to flush stagingVertexBuffer memory because CPU_ONLY memory is always HOST_COHERENT.

    vk::ImageCreateInfo imageCreateInfo(
            {},
            vk::ImageType::e2D,
            format,
            extent,
            1,
            1,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
            vk::SharingMode::eExclusive,
            0,
            nullptr,
            vk::ImageLayout::eUndefined
    );

    allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    allocationCreateInfo.flags = 0;

    if (vmaCreateImage(allocator, reinterpret_cast<const VkImageCreateInfo *>(&imageCreateInfo), &allocationCreateInfo,
                       reinterpret_cast<VkImage *>(&_image), &_allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Error: vmaCreateImage");
    }

    vk::CommandBufferAllocateInfo commandBufferInfo(
            commandPool,
            vk::CommandBufferLevel::ePrimary,
            1
    );

    vk::CommandBuffer transferCommandBuffer;
    if (device.allocateCommandBuffers(&commandBufferInfo, &transferCommandBuffer) != vk::Result::eSuccess) {
        throw std::runtime_error("Error: vk::AllocateCommandBuffers");
    }

    vk::CommandBufferBeginInfo transferCmdBeginInfo(
            vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
            nullptr
    );

    if (transferCommandBuffer.begin(&transferCmdBeginInfo) != vk::Result::eSuccess) {
        throw std::runtime_error("Error: vk::BeginCommandBuffer");
    }

    vk::ImageSubresourceRange range(
            vk::ImageAspectFlagBits::eColor,
            0,
            1,
            0,
            1
    );

    vk::ImageMemoryBarrier imageBarrier(
            vk::AccessFlagBits::eNone,
            vk::AccessFlagBits::eTransferWrite,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal,
            vk::QueueFamilyIgnored,
            vk::QueueFamilyIgnored,
            _image,
            range
    );

    transferCommandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eTransfer,
            {},
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageBarrier);

    vk::ImageSubresourceLayers layer(
            vk::ImageAspectFlagBits::eColor,
            0,
            0,
            1
    );

    vk::BufferImageCopy bufferImageCopy(
            0,
            0,
            0,
            layer,
            {0, 0},
            extent
    );

    transferCommandBuffer.copyBufferToImage(
            stagingBuffer,
            _image,
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &bufferImageCopy
    );

    imageBarrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    imageBarrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

    imageBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    imageBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    transferCommandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            {},
            0,
            nullptr,
            0,
            nullptr,
            1,
            &imageBarrier
    );

    // todo: exception?
    transferCommandBuffer.end();

    vk::SubmitInfo submitTransferInfo(
            0, nullptr,
            nullptr,
            1, &transferCommandBuffer,
            0, nullptr
    );

    if (graphicsQueue.submit(1, &submitTransferInfo, nullptr) != vk::Result::eSuccess) {
        throw std::runtime_error("Error: vk::QueueSubmit");
    }

    // todo: exception?
    graphicsQueue.waitIdle();

    device.freeCommandBuffers(commandPool, 1, &transferCommandBuffer);
    vmaDestroyBuffer(allocator, stagingBuffer, stagingBufferAlloc);

    vk::ImageViewCreateInfo viewCreateInfo(
            {},
            _image,
            vk::ImageViewType::e2D,
            format,
            vk::ComponentMapping{
                    vk::ComponentSwizzle::eR,
                    vk::ComponentSwizzle::eG,
                    vk::ComponentSwizzle::eB,
                    vk::ComponentSwizzle::eA
            },
            vk::ImageSubresourceRange(
                    vk::ImageAspectFlagBits::eColor,
                    0,
                    1,
                    0,
                    1
            )
    );

    if (device.createImageView(&viewCreateInfo, nullptr, &_imageView) != vk::Result::eSuccess) {
        throw std::runtime_error("Error: vk::CreateImageView");
    }
}

Image::Image(vk::Image image, vk::ImageView imageView) {
    _image = image;
    _imageView = imageView;
}


Image::Image(vk::Device device, VmaAllocator allocator, const ImageCreateInfo &imageCreateInfo) {
    vk::ImageCreateInfo createInfo(
            {},
            imageCreateInfo.imageType,
            imageCreateInfo.format,
            imageCreateInfo.extent,
            1,
            1,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            imageCreateInfo.usage,
            vk::SharingMode::eExclusive,
            0,
            nullptr,
            vk::ImageLayout::eUndefined
    );

    VmaAllocationCreateInfo allocationCreateInfo{
            .flags = {},
            .usage = VMA_MEMORY_USAGE_GPU_ONLY,
            .requiredFlags = {},
            .preferredFlags = {},
            .memoryTypeBits = {},
            .pool = {},
            .pUserData = nullptr,
    };

    if (vmaCreateImage(allocator, reinterpret_cast<const VkImageCreateInfo *>(&createInfo), &allocationCreateInfo,
                       reinterpret_cast<VkImage *>(&_image), &_allocation, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Error: vmaCreateImage");
    }

    vk::ImageViewCreateInfo viewCreateInfo(
            {},
            _image,
            imageCreateInfo.viewType,
            imageCreateInfo.format,
            vk::ComponentMapping{
                    vk::ComponentSwizzle::eR,
                    vk::ComponentSwizzle::eG,
                    vk::ComponentSwizzle::eB,
                    vk::ComponentSwizzle::eA
            },
            vk::ImageSubresourceRange(
                    imageCreateInfo.aspectMask,
                    0,
                    1,
                    0,
                    1
            )
    );

    if (device.createImageView(&viewCreateInfo, nullptr, &_imageView) != vk::Result::eSuccess) {
        throw std::runtime_error("Error: vk::CreateImageView");
    }
}

void Image::destroy(vk::Device device, VmaAllocator allocator) {
    device.destroy(_imageView);
    vmaDestroyImage(allocator, _image, _allocation);
}

vk::ImageView Image::getImageView() const {
    return _imageView;
}

vk::Image Image::getImage() const {
    return _image;
}