#pragma once

#include "VkBootstrap.h"
#include "ShaderManager.h"
#include <memory>
#include <optional>

class Pipeline;

class PipelineLayoutManager;

struct VertexAttributeDescription;

class PipelineManager {
public:
    PipelineManager(VkDevice device, float width, float height,
                    std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager);

    std::optional<std::shared_ptr<Pipeline>> getPipeline(const std::vector<VertexAttributeDescription> &descriptions,
                                          const std::vector<VkDescriptorType> &types, const std::string &shaderName);

    // TODO: Remove all destroys
    void destroy();

private:
    VkDevice device;

    float width;
    float height;

    std::vector<std::shared_ptr<Pipeline>> pipelines;
    std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager;

    ShaderManager shaderManager;
};
