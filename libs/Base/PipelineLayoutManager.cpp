#include "PipelineLayoutManager.h"
#include "../Base/DescriptorSetLayout.h"
#include "DescriptorSetLayoutManager.h"
#include "../Base/PipelineLayout.h"
#include "../Base/PushData.h"

PipelineLayoutManager::PipelineLayoutManager(const vkb::Device &device,
                                             std::shared_ptr<DescriptorSetLayoutManager> descriptorSetLayoutManager)
        : _device(device), _descriptorSetLayoutManager(descriptorSetLayoutManager) {
}

std::shared_ptr<PipelineLayout> PipelineLayoutManager::getLayout(const std::vector<VkDescriptorType> &types) {
    auto found = std::find_if(_layouts.begin(), _layouts.end(), [&](const std::shared_ptr<PipelineLayout> &layout) {
        return layout->getDescriptorSetLayout()->isCompatible(types);
    });

    if (found != _layouts.end()) {
        return *found;
    }

    std::shared_ptr<DescriptorSetLayout> descriptorSetLayout = _descriptorSetLayoutManager->getLayout(types);

    auto layout = std::make_shared<PipelineLayout>(_device, static_cast<uint32_t>(sizeof(PushData)),
                                                   descriptorSetLayout);
    _layouts.push_back(layout);

    return layout;
}

void PipelineLayoutManager::destroy() {
    _layouts.clear();
}
