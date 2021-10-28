#pragma once
#include "Image.h"
#include "VkResource.h"

class Texture : public VkResource<Texture> {
public:
    Texture() = default;
    Texture(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool, const std::string& path);

    void destroy(VkDevice device, VmaAllocator allocator);

    VkImageView getImageView() const;
    VkSampler getSampler() const;

private:
    Image _image;
    VkSampler _sampler;
};
