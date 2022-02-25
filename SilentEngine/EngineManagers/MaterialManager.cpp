#include "MaterialManager.h"
#include "DescriptorSetManager.h"
#include "../Material.h"
#include "PipelineManager.h"

MaterialManager::MaterialManager(const vkb::Device &device, std::shared_ptr<PipelineManager> pipelineManager,
                                 std::shared_ptr<DescriptorSetManager> descriptorSetManager)
        : _device(device), _pipelineManager(pipelineManager), _descriptorSetManager(descriptorSetManager) {
}

MaterialManager::~MaterialManager() {
}

std::shared_ptr<Material> MaterialManager::getMaterial(const std::vector<VertexAttributeDescription> &descriptions,
                                                       const std::vector<VkDescriptorType> &types,
                                                       std::vector<std::shared_ptr<Texture>> &textures) {
    auto pipeline = _pipelineManager->getPipeline(descriptions, types);
    auto descriptorSet = _descriptorSetManager->getDescriptorSet(types, textures);

    auto found = std::find_if(_materials.begin(), _materials.end(), [&](std::shared_ptr<Material> material) {
        return material->isCompatible(pipeline, descriptorSet);
    });

    if (found != _materials.end()) {
        return *found;
    }

    auto material = std::make_shared<Material>(pipeline, descriptorSet);
    _materials.push_back(material);

    return material;
}

void MaterialManager::destroy() {
    _materials.clear();
}
