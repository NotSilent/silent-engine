#include "PipelineManager.h"
#include "Pipeline.h"
#include "PipelineLayoutManager.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <ranges>

PipelineManager::PipelineManager(VkDevice device, float width, float height,
                                 std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager)
        : device(device)
        , width(width)
        , height(height)
        , pipelineLayoutManager(std::move(pipelineLayoutManager))
        , shaderManager(device){
}

std::shared_ptr<Pipeline> PipelineManager::getPipeline(const std::vector<VertexAttributeDescription> &descriptions,
                                                       const std::vector<VkDescriptorType> &types,
                                                       const std::string &shaderName) {
    auto pipelineLayout = pipelineLayoutManager->getLayout(types);

    auto found = std::find_if(pipelines.begin(), pipelines.end(), [&](std::shared_ptr<Pipeline> &pipeline) {
        return pipeline->isCompatible(descriptions, pipelineLayout);
    });

    if (found != pipelines.end()) {
        return *found;
    }

    std::optional<Shader> shader = shaderManager.getShader(shaderName);

    if(shader.has_value())
    {
        auto pipeline = std::make_shared<Pipeline>(device, width, height, descriptions, pipelineLayout,
                                                   shader.value());
        pipelines.push_back(pipeline);

        return pipeline;
    }

    return {};
}

void PipelineManager::destroy() {
    pipelines.clear();

    shaderManager.destroy();
}