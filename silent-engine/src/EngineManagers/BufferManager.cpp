#include "BufferManager.h"

BufferManager::BufferManager(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool commandPool)
    : _device(device)
    , _allocator(allocator)
    , _commandPool(commandPool)
{
}

std::shared_ptr<Buffer> BufferManager::getBuffer(const std::string& name)
{
    // TODO: Error checking
    return _buffers[name];
}

void BufferManager::destroy()
{
    for (auto& buffer : _buffers) {
        buffer.second->destroy(_device.device, _allocator);
    }
}

void BufferManager::addBuffer(const std::string& name, uint32_t sizeBytes, const void* data)
{
    if(_buffers.contains(name)) {
        // TODO: Make less-error prone
        return;
    }

    VkBufferUsageFlags flags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    _buffers[name] = std::make_shared<Buffer>(_device, _allocator, _commandPool, flags, sizeBytes, data);
}
