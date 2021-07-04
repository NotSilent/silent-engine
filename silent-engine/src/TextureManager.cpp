#include "TextureManager.h"

#include <memory>

TextureManager::TextureManager(VkDevice device, VmaAllocator allocator, VkCommandPool commandPool, VkQueue queue)
    : _device(device)
    , _allocator(allocator)
    , _commandPool(commandPool)
    , _queue(queue)
{
}

void TextureManager::addTexture(const std::string& path)
{
    if (_textures.contains(path)) {
        return;
    }

    _textures[path] = std::make_shared<Texture>(_device, _allocator, _commandPool, _queue, path);
}

std::shared_ptr<Texture> TextureManager::getTexture(const std::string& path)
{
    return _textures[path];
}

void TextureManager::destroy()
{
    for(auto& texture : _textures) {
        texture.second->destroy(_device, _allocator);
    }
}
