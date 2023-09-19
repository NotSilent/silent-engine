#include "MaterialManager.h"
#include "DescriptorSetManager.h"
#include "Material.h"
#include "PipelineManager.h"
#include <algorithm>

MaterialManager::MaterialManager(const vkb::Device &device, std::shared_ptr<PipelineManager> pipelineManager,
                                 std::shared_ptr<DescriptorSetManager> descriptorSetManager)
        : _device(device), _pipelineManager(pipelineManager), _descriptorSetManager(descriptorSetManager) {
}

std::optional<std::shared_ptr<Material>> MaterialManager::getMaterial(const std::vector<VertexAttributeDescription> &descriptions,
                                                       const std::vector<VkDescriptorType> &types,
                                                       std::vector<std::shared_ptr<Texture>> &textures) {
    std::optional<std::shared_ptr<Pipeline>> pipeline = _pipelineManager->getPipeline(descriptions, types, "unlit");
    std::shared_ptr<DescriptorSet> descriptorSet = _descriptorSetManager->getDescriptorSet(types, textures);

    if(pipeline.has_value())
    {
        auto found = std::find_if(_materials.begin(), _materials.end(), [&](const std::shared_ptr<Material>& material) {
            return material->isCompatible(pipeline.value(), descriptorSet);
        });

        if (found != _materials.end()) {
            return *found;
        }

        auto material = std::make_shared<Material>(pipeline.value(), descriptorSet);
        _materials.push_back(material);

        return material;
    }

    return {};
}

void MaterialManager::destroy() {
    _materials.clear();
}
