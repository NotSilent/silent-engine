#include "DescriptorSet.h"
#include "DescriptorSetLayout.h";
#include "Texture.h"

DescriptorSet::DescriptorSet(const vkb::Device& device, VkDescriptorPool descriptorPool, std::shared_ptr<DescriptorSetLayout> layout, std::vector<std::shared_ptr<Texture>>& textures)
    : _device(device)
    , _layout(layout)
    , _textures(textures)
{
    VkDescriptorSetLayout vkLayout = _layout->getLayout();

    VkDescriptorSetAllocateInfo allocateInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &vkLayout,
    };

    if (vkAllocateDescriptorSets(device.device, &allocateInfo, &_descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateDescriptorSetLayout");
    }

    std::vector<VkWriteDescriptorSet> writes;
    for (uint32_t i = 0; i < _textures.size(); ++i) {

        VkDescriptorImageInfo imageInfo {
            .sampler = _textures[i]->getSampler(),
            .imageView = _textures[i]->getImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        writes.push_back({
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = _descriptorSet,
            .dstBinding = i,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr,
        });

        vkUpdateDescriptorSets(device.device, writes.size(), writes.data(), 0, nullptr);
    }
}

DescriptorSet::~DescriptorSet()
{
    // Allocated from pool
}

VkDescriptorSet DescriptorSet::getDescriptorSet() const
{
    return _descriptorSet;
}

bool DescriptorSet::isCompatible(std::shared_ptr<DescriptorSetLayout> layout, std::vector<std::shared_ptr<Texture>>& textures)
{
    return _layout == layout && _textures == textures;
}
