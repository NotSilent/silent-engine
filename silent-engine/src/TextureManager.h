#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "Texture.h"

class TextureManager {
public:
    TextureManager() = default;

    TextureManager(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool);

    void addTexture(const std::string& path);

    std::shared_ptr<Texture> getTexture(const std::string& path);

    void destroy();

private:
    vkb::Device _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Texture>> _textures {};

};
