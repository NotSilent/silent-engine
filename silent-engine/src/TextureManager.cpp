#include "TextureManager.h"

#include <memory>

TextureManager::TextureManager(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool)
    : _device(device)
    , _allocator(allocator)
    , _commandPool(commandPool)
{
}

void TextureManager::addTexture(const std::string& path)
{
    if (_textures.contains(path)) {
        return;
    }

    _textures[path] = std::make_shared<Texture>(_device, _allocator, _commandPool, path);
}

std::shared_ptr<Texture> TextureManager::getTexture(const std::string& path)
{
    if (!_textures.contains(path)) {
        addTexture(path);
    }

    return _textures[path];
}

void TextureManager::destroy()
{
    for(auto& texture : _textures) {
        texture.second->destroy(_device.device, _allocator);
    }
}
