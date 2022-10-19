#include "Sampler.h"

Sampler::Sampler(const vkb::Device &device) {
    _device = device;

    VkSamplerCreateInfo createInfo{
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
            .maxAnisotropy = 1.0f,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = 0.0f,
            .maxLod = 1.0f,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
            .unnormalizedCoordinates = VK_FALSE,
    };

    if (vkCreateSampler(device.device, &createInfo, nullptr, &_sampler) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateSampler");
    }
}

Sampler::~Sampler() {
}

VkSampler Sampler::getSampler() const {
    return _sampler;
}

void Sampler::destroy(VkDevice device, VmaAllocator allocator) {
    vkDestroySampler(device, _sampler, nullptr);
}
