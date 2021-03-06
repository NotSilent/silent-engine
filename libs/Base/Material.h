#pragma once
#include "VertexAttribute.h"
#include <string>
#include <vector>

class Pipeline;
class DescriptorSet;

class Material {
public:
    Material(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<DescriptorSet> descriptorSet);
    ~Material();

    bool isCompatible(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<DescriptorSet> descriptor);

    VkDescriptorSet getDescriptorSet() const;
    VkPipeline getPipeline() const;
    VkPipelineLayout getPipelineLayout() const;

private:
    std::shared_ptr<Pipeline> _pipeline;
    std::shared_ptr<DescriptorSet> _descriptorSet;
};
