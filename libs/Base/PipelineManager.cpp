#include "PipelineManager.h"
#include "Pipeline.h"
#include "PipelineLayoutManager.h"
#include <algorithm>
#include <utility>
#include <vector>
#include <ranges>

PipelineManager::PipelineManager(VkDevice device, VkFormat swapchainFormat, float width, float height,
                                 std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager)
        : device(device)
        , swapchainFormat(swapchainFormat)
        , width(width)
        , height(height)
        , pipelineLayoutManager(std::move(pipelineLayoutManager))
        , shaderManager(device){
}

std::shared_ptr<Pipeline> PipelineManager::getPipeline(const std::string &shaderName) {
    auto pipelineLayout = pipelineLayoutManager->getLayout();

    auto found = std::find_if(pipelines.begin(), pipelines.end(), [&](std::shared_ptr<Pipeline> &pipeline) {
        return pipeline->isCompatible(pipelineLayout);
    });

    if (found != pipelines.end()) {
        return *found;
    }

    std::optional<Shader> shader = shaderManager.getShader(shaderName);

    if(shader.has_value())
    {
        auto pipeline = std::make_shared<Pipeline>(device, width, height, pipelineLayout,
                                                   swapchainFormat, shader.value());
        pipelines.push_back(pipeline);

        return pipeline;
    }

    return {};
}

void PipelineManager::destroy() {
    pipelines.clear();

    shaderManager.destroy();
}