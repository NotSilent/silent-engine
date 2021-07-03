#pragma once
#include <memory>
#include <string>
#include <unordered_map>


#include "Image.h"

class ImageManager {
public:
private:
    vkb::Device _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Image>> _images {};

public:
    ImageManager() = default;

    ImageManager(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool commandPool);

    void addImage(const std::string& path);

    std::weak_ptr<Image> getImage(const std::string& path);

    void release();

private:
};
