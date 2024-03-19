#include "ImageManager.h"

ImageManager::ImageManager(const vk::Device &device, vk::Queue graphicsQueue, VmaAllocator allocator, vk::CommandPool commandPool)
        : _device(device), graphicsQueue(graphicsQueue), _allocator(allocator), _commandPool(commandPool) {
}

std::shared_ptr<Image> ImageManager::getImage(const std::string &name) {
    // TODO: Error checking
    return _images[name];
}

void ImageManager::destroy() {
    for (auto &image: _images) {
        image.second->destroy(_device, _allocator);
    }
}


void ImageManager::addImage(const std::string &name, uint32_t width, uint32_t height, uint32_t size, const void *data) {
    if (_images.contains(name)) {
        // TODO: Make less-error prone
        return;
    }

    vk::Format format = vk::Format::eR8G8B8A8Srgb;
    _images[name] = std::make_shared<Image>(_device, graphicsQueue, _allocator, _commandPool, width, height, format, size, data);
}