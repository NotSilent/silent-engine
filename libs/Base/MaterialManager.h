#pragma once
#include "VkBootstrap.h"
#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <optional>

class Material;
class PipelineManager;
class Texture;
struct VertexAttributeDescription;

class MaterialManager {
public:
    MaterialManager() = default;
    MaterialManager(VkDevice device, std::shared_ptr<PipelineManager> pipelineManager);

    std::optional<std::shared_ptr<Material>> getMaterial(const std::vector<VertexAttributeDescription>& descriptions, const std::vector<VkDescriptorType>& types, std::vector<std::shared_ptr<Texture>>& textures);

    // TODO: Remove all destroys
    void destroy();

private:
    VkDevice device = nullptr;

    std::vector<std::shared_ptr<Material>> _materials;

    std::shared_ptr<PipelineManager> _pipelineManager;
};
