#pragma once

#include "../Base/Image.h"
#include <memory>
#include <string>
#include <unordered_map>

class ImageManager {
public:
    ImageManager() = default;

    ImageManager(const vkb::Device &device, const VmaAllocator allocator, const VkCommandPool commandPool);

    void addImage(const std::string &name, uint32_t width, uint32_t height, uint32_t size, const void *data);

    std::shared_ptr<Image> getImage(const std::string &name);

    void destroy();

private:
    vkb::Device _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Image>> _images;
};
