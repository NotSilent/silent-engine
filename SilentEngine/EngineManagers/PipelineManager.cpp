#include "PipelineManager.h"
#include "../Pipeline.h"
#include "PipelineLayoutManager.h"

PipelineManager::PipelineManager(const vkb::Device &device, float width, float height, VkRenderPass renderPass,
                                 std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager)
        : _device(device), _width(width), _height(height), _renderPass(renderPass),
          _pipelineLayoutManager(pipelineLayoutManager) {
}

std::shared_ptr<Pipeline> PipelineManager::getPipeline(const std::vector<VertexAttributeDescription> &descriptions,
                                                       const std::vector<VkDescriptorType> &types) {
    auto pipelineLayout = _pipelineLayoutManager->getLayout(types);

    auto found = std::find_if(_pipelines.begin(), _pipelines.end(), [&](std::shared_ptr<Pipeline> pipeline) {
        return pipeline->isCompatible(descriptions, pipelineLayout);
    });

    if (found != _pipelines.end()) {
        return *found;
    }

    auto pipeline = std::make_shared<Pipeline>(_device, _width, _height, _renderPass, descriptions, pipelineLayout);
    _pipelines.push_back(pipeline);

    return pipeline;
}

void PipelineManager::destroy() {
    _pipelines.clear();
}
