#include "PipelineLayoutManager.h"
#include "PipelineLayout.h"
#include "PushData.h"
#include <algorithm>

PipelineLayoutManager::PipelineLayoutManager(VkDevice device)
        : device(device) {
}

std::shared_ptr<PipelineLayout> PipelineLayoutManager::getLayout(const std::vector<VkDescriptorType> &types) {
    auto layout = std::make_shared<PipelineLayout>(device, sizeof(PushData));
    _layouts.push_back(layout);

    return layout;
}

void PipelineLayoutManager::destroy() {
    _layouts.clear();
}
