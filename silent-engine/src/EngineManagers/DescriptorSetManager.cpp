#include "DescriptorSetManager.h"
#include "DescriptorSet.h"
#include "DescriptorSetLayoutManager.h"
#include "Texture.h"

DescriptorSetManager::DescriptorSetManager(const vkb::Device& device, std::shared_ptr<DescriptorSetLayoutManager> descriptorSetLayoutManager)
    : _device(device)
    , _descriptorSetLayoutManager(descriptorSetLayoutManager)
{
    VkDescriptorPoolSize descriptorPoolSizes[] {
        {
            .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            .descriptorCount = 500,
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 500,
        },
    };

    VkDescriptorPoolCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .maxSets = 1000,
        .poolSizeCount = 2,
        .pPoolSizes = descriptorPoolSizes,
    };

    if (vkCreateDescriptorPool(device.device, &createInfo, nullptr, &_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("Error: vkCreateDescriptorPool");
    }
}

std::shared_ptr<DescriptorSet> DescriptorSetManager::getDescriptorSet(const std::vector<VkDescriptorType> types, std::vector<std::shared_ptr<Texture>>& textures)
{
    auto layout = _descriptorSetLayoutManager->getLayout(types);

    auto found = std::find_if(_descriptorSets.begin(), _descriptorSets.end(), [&](std::shared_ptr<DescriptorSet> descriptorSet) {
        return descriptorSet->isCompatible(layout, textures);
    });

    if (found != _descriptorSets.end()) {
        return *found;
    }

    auto descriptorSet = std::make_shared<DescriptorSet>(_device, _descriptorPool, layout, textures);
    _descriptorSets.push_back(descriptorSet);

    return descriptorSet;
}

void DescriptorSetManager::destroy()
{
    _descriptorSets.clear();
    vkDestroyDescriptorPool(_device.device, _descriptorPool, nullptr);
}
