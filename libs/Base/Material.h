#pragma once

#include "VertexAttribute.h"
#include <memory>
#include <string>
#include <vector>

class Pipeline;

class Material {
public:
    explicit Material(std::shared_ptr<Pipeline> pipeline);

    bool isCompatible(const std::shared_ptr<Pipeline>& pipeline);

    [[nodiscard]] VkPipeline getPipeline() const;

    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

private:
    std::shared_ptr<Pipeline> _pipeline;
};
