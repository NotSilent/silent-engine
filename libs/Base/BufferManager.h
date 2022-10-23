#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include "../Base/Buffer.h"

class BufferManager {
public:
    BufferManager() = default;

    BufferManager(const vkb::Device &device, VmaAllocator allocator, VkCommandPool commandPool);

    void addBuffer(const std::string &name, uint32_t sizeBytes, const void *data);

    std::shared_ptr<Buffer> getBuffer(const std::string &name);

    void destroy();

private:
    vkb::Device _device;
    VmaAllocator _allocator = VK_NULL_HANDLE;
    VkCommandPool _commandPool = VK_NULL_HANDLE;

    std::unordered_map<std::string, std::shared_ptr<Buffer>> _buffers;
};
