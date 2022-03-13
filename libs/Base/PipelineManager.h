#pragma once

#include "VkBootstrap.h"
#include <memory>

class Pipeline;

class PipelineLayoutManager;

struct VertexAttributeDescription;

class PipelineManager {
public:
    PipelineManager(const vkb::Device &device, float width, float height, VkRenderPass renderPass,
                    std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager);

    std::shared_ptr<Pipeline> getPipeline(const std::vector<VertexAttributeDescription> &descriptions,
                                          const std::vector<VkDescriptorType> &types);

    // TODO: Remove all destroys
    void destroy();

private:
    vkb::Device _device;

    //TODO: RenderPass
    float _width;
    float _height;
    VkRenderPass _renderPass;

    std::vector<std::shared_ptr<Pipeline>> _pipelines;
    std::shared_ptr<PipelineLayoutManager> _pipelineLayoutManager;
};
