#pragma once

#include "Image.h"
#include <memory>
#include <string>
#include <unordered_map>

class ImageManager {
public:
    ImageManager() = default;

    ImageManager(const vk::Device &device, vk::Queue graphicsQueue, VmaAllocator allocator, vk::CommandPool commandPool);

    void addImage(const std::string &name, uint32_t width, uint32_t height, uint32_t size, const void *data);

    std::shared_ptr<Image> getImage(const std::string &name);

    void destroy();

private:
    vk::Device _device;
    vk::Queue graphicsQueue;
    VmaAllocator _allocator;
    vk::CommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Image>> _images;
};
