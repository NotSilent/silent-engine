#include "TextureManager.h"

#include <memory>

TextureManager::TextureManager(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool)
    : _device(device)
    , _allocator(allocator)
    , _commandPool(commandPool)
{
}

void TextureManager::addTexture(const std::string& name, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image)
{
    if (_textures.contains(name)) {
        return;
    }

    _textures[name] = std::make_shared<Texture>(_device, _allocator, _commandPool, sampler, image);
}

std::shared_ptr<Texture> TextureManager::getTexture(const std::string& name)
{
    // TODO: error checking
    return _textures[name];
}

void TextureManager::destroy()
{
    for(auto& texture : _textures) {
        texture.second->destroy(_device.device, _allocator);
    }
}
