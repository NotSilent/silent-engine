#pragma once
#include "Image.h"
#include "VkResource.h"
#include <memory>
#include <Sampler.h>

class Texture : public VkResource<Texture> {
public:
    Texture() = default;
    Texture(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image);

    void destroy(VkDevice device, VmaAllocator allocator);

    VkImageView getImageView() const;
    VkSampler getSampler() const;

private:
    std::shared_ptr<Sampler> _sampler;
    std::shared_ptr<Image> _image;
};
