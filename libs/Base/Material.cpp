#include "Material.h"

#include <utility>
#include "Pipeline.h"

Material::Material(std::shared_ptr<Pipeline> pipeline)
        : _pipeline(std::move(pipeline)) {
}

bool Material::isCompatible(const std::shared_ptr<Pipeline>& pipeline) {
    return _pipeline == pipeline;
}

VkPipeline Material::getPipeline() const {
    return _pipeline->getPipeline();
}

VkPipelineLayout Material::getPipelineLayout() const {
    return _pipeline->getPipelineLayout();
}
