#include "DescriptorSetLayoutManager.h"
#include "DescriptorSetLayout.h"
#include <algorithm>

DescriptorSetLayoutManager::DescriptorSetLayoutManager(const vkb::Device &device)
        : _device(device) {
}

std::shared_ptr<DescriptorSetLayout> DescriptorSetLayoutManager::getLayout(const std::vector<VkDescriptorType> &types) {
    auto found = std::find_if(_layouts.begin(), _layouts.end(), [&](std::shared_ptr<DescriptorSetLayout> layout) {
        return layout->isCompatible(types);
    });

    if (found != _layouts.end()) {
        return *found;
    }

    auto layout = std::make_shared<DescriptorSetLayout>(_device, types);
    _layouts.push_back(layout);

    return layout;
}

void DescriptorSetLayoutManager::destroy() {
    _layouts.clear();
}
