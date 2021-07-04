#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "Texture.h"

class TextureManager {
public:
    TextureManager() = default;

    TextureManager(VkDevice device, VmaAllocator allocator, VkCommandPool commandPool, VkQueue queue);

    void addTexture(const std::string& path);

    std::shared_ptr<Texture> getTexture(const std::string& path);

    void destroy();

private:
    VkDevice _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;
    VkQueue _queue;

    std::unordered_map<std::string, std::shared_ptr<Texture>> _textures {};

};
