#include "MaterialManager.h"
#include "Material.h"
#include "PipelineManager.h"
#include <algorithm>
#include <utility>

MaterialManager::MaterialManager(VkDevice device, std::shared_ptr<PipelineManager> pipelineManager)
        : device(device)
        , _pipelineManager(std::move(pipelineManager)) {
}

std::optional<std::shared_ptr<Material>> MaterialManager::getMaterial(const std::vector<VertexAttributeDescription> &descriptions,
                                                       const std::vector<VkDescriptorType> &types,
                                                       std::vector<std::shared_ptr<Texture>> &textures) {
    std::optional<std::shared_ptr<Pipeline>> pipeline = _pipelineManager->getPipeline(descriptions, types, "unlit");

    if(pipeline.has_value())
    {
        auto found = std::find_if(_materials.begin(), _materials.end(), [&](const std::shared_ptr<Material>& material) {
            return material->isCompatible(pipeline.value());
        });

        if (found != _materials.end()) {
            return *found;
        }

        auto material = std::make_shared<Material>(pipeline.value());
        _materials.push_back(material);

        return material;
    }

    return {};
}

void MaterialManager::destroy() {
    _materials.clear();
}
