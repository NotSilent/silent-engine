#pragma once
#include "Texture.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <Sampler.h>

class TextureManager {
public:
    TextureManager() = default;

    TextureManager(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool);

    void addTexture(const std::string& name, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image);

    std::shared_ptr<Texture> getTexture(const std::string& name);

    void destroy();

private:
    vkb::Device _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Texture>> _textures {};
};
