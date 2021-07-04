#include "Texture.h"

Texture::Texture(VkDevice device, VmaAllocator allocator, VkCommandPool commandPool, VkQueue queue, const std::string& path)
{
    _image = Image(device, allocator, commandPool, queue, path);

    VkSamplerCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0.0f,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_NEVER,
        .minLod = 0.0f,
        .maxLod = 0.0f,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
    };

    if (vkCreateSampler(device, &createInfo, nullptr, &_sampler) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateSampler");
    }
}

void Texture::destroy(VkDevice device, VmaAllocator allocator)
{
    vkDestroySampler(device, _sampler, nullptr);
    _image.destroy(device, allocator);
}

VkImageView Texture::getImageView() const
{
    return _image.getImageView();
}

VkSampler Texture::getSampler() const
{
    return _sampler;
}
