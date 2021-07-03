#pragma once
#include "Image.h"

class Texture {
public:
    Texture() = default;
    Texture(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool pool, const std::string& path);

    ~Texture();

private:
    Image _image;
    VkSampler _sampler;
};
