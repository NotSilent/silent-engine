#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include "Buffer.h"

class BufferManager {
public:
    BufferManager() = default;
    BufferManager(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool commandPool);
    
    void addBuffer(const std::string& name, uint32_t sizeBytes, const void* data);

    std::shared_ptr<Buffer> getBuffer(const std::string& name);
    
    void destroy();

private:
    vkb::Device _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Buffer>> _buffers;
};
