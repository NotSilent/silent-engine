#pragma once

#include "VkBootstrap.h"
#include <memory>

class Pipeline;

class PipelineLayoutManager;

struct VertexAttributeDescription;

class PipelineManager {
public:
    PipelineManager(const vkb::Device &device, float width, float height,
                    std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager);

    std::shared_ptr<Pipeline> getPipeline(const std::vector<VertexAttributeDescription> &descriptions,
                                          const std::vector<VkDescriptorType> &types, const std::string &shaderName);

    // TODO: Remove all destroys
    void destroy();

private:
    vkb::Device _device;

    float _width;
    float _height;

    std::vector<std::shared_ptr<Pipeline>> _pipelines;
    std::shared_ptr<PipelineLayoutManager> _pipelineLayoutManager;
};
