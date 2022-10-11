#include "Texture.h"

Texture::Texture(const std::shared_ptr<Sampler>& sampler, const std::shared_ptr<Image>& image)
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
