#include "BufferManager.h"

BufferManager::BufferManager(const vkb::Device &device, VmaAllocator allocator, VkCommandPool commandPool)
        : _device(device), _allocator(allocator), _commandPool(commandPool) {
}

VkBuffer BufferManager::getBuffer(const std::string &name) {
    // TODO: Error checking
    return _buffers[name].getBuffer();
}

void BufferManager::destroy() {
    for (auto &[_, buffer]: _buffers) {
        buffer.destroy(_device, _allocator);
    }

    _buffers.clear();
}

void BufferManager::addBuffer(const std::string &name, VkBufferUsageFlags flags, uint32_t sizeBytes, const void *data) {
    if (_buffers.contains(name)) {
        // TODO: Make less-error prone
        return;
    }

    _buffers[name] = Buffer(_device, _allocator, _commandPool, flags, sizeBytes, data);
}
