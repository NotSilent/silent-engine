#pragma once

#include "VkBootstrap.h"
#include "vk_mem_alloc.h"

class Buffer {
private:
    VmaAllocator _allocator;

    VkBuffer _buffer;
    VmaAllocation _allocation;

    uint32_t _sizeBytes;

public:
    Buffer() = default;

    Buffer(const vkb::Device &device, VmaAllocator allocator, VkCommandPool commandPool, VkBufferUsageFlags usage,
           uint32_t size, const void *data);

    Buffer(const Buffer &other) = delete;

    Buffer operator=(const Buffer &other) = delete;

    Buffer(Buffer &&other) = default;

    Buffer &operator=(Buffer &&other) = default;

    [[nodiscard]] VkBuffer getBuffer() const;

    void destroy(const VkDevice device, const VmaAllocator &allocator);
};
