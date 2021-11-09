#include "Texture.h"

Texture::Texture(const vkb::Device& device, VmaAllocator allocator, VkCommandPool commandPool, std::shared_ptr<Sampler> sampler, std::shared_ptr<Image> image)
{
    _sampler = sampler;
    _image = image;
}

void Texture::destroy(VkDevice device, VmaAllocator allocator)
{
}

VkImageView Texture::getImageView() const
{
    return _image->getImageView();
}

VkSampler Texture::getSampler() const
{
    return _sampler->getSampler();
}
