#include "Material.h"
#include "DescriptorSet.h"
#include "Pipeline.h"

Material::Material(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<DescriptorSet> descriptorSet)
        : _pipeline(pipeline), _descriptorSet(descriptorSet) {
}

Material::~Material() {
}

bool Material::isCompatible(std::shared_ptr<Pipeline> pipeline, std::shared_ptr<DescriptorSet> descriptor) {
    return _pipeline == pipeline && _descriptorSet == descriptor;
}

VkDescriptorSet Material::getDescriptorSet() const {
    return _descriptorSet->getDescriptorSet();
}

VkPipeline Material::getPipeline() const {
    return _pipeline->getPipeline();
}

VkPipelineLayout Material::getPipelineLayout() const {
    return _pipeline->getPipelineLayout();
}
