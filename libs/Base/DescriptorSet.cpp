#include "DescriptorSet.h"
#include "DescriptorSetLayout.h"
#include "Texture.h"

DescriptorSet::DescriptorSet(const vkb::Device &device, VkDescriptorPool descriptorPool,
                             std::shared_ptr<DescriptorSetLayout> layout,
                             const std::vector<std::shared_ptr<Texture>> &textures)
        : _device(device), _layout(layout), _textures(textures) {
    VkDescriptorSetLayout vkLayout = _layout->getLayout();

    VkDescriptorSetAllocateInfo allocateInfo{
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
    std::vector<VkDescriptorImageInfo> imageInfos;
    const std::vector<VkDescriptorType> &types = _layout->getDescriptorTypes();
    for (uint32_t i = 0; i < types.size(); ++i) {
        imageInfos.push_back({
                                     .sampler = types[i] == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
                                                ? _textures[i]->getSampler() : VK_NULL_HANDLE,
                                     .imageView = _textures[i]->getImageView(),
                                     .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                             });
    }
    for (uint32_t i = 0; i < types.size(); ++i) {
        writes.push_back({
                                 .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                                 .pNext = nullptr,
                                 .dstSet = _descriptorSet,
                                 .dstBinding = i,
                                 .dstArrayElement = 0,
                                 .descriptorCount = 1,
                                 .descriptorType = types[i],
                                 .pImageInfo = &imageInfos[i],
                                 .pBufferInfo = nullptr,
                                 .pTexelBufferView = nullptr,
                         });
    }


    if (!textures.empty()) {
        vkUpdateDescriptorSets(device.device, writes.size(), writes.data(), 0, nullptr);
    }
}

DescriptorSet::~DescriptorSet() {
    // Allocated from pool
}

VkDescriptorSet DescriptorSet::getDescriptorSet() const {
    return _descriptorSet;
}

bool DescriptorSet::isCompatible(std::shared_ptr<DescriptorSetLayout> layout,
                                 const std::vector<std::shared_ptr<Texture>> &textures) {
    return _layout == layout && _textures == textures;
}
