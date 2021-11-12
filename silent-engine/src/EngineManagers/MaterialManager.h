#pragma once
#include "vk-bootstrap\VkBootstrap.h"
#include <memory>
#include <vector>
#include <vulkan\vulkan.h>

class Material;
class PipelineManager;
class DescriptorSetManager;
class Texture;
struct VertexAttributeDescription;

class MaterialManager {
public:
    MaterialManager() = default;
    MaterialManager(const vkb::Device& device, std::shared_ptr<PipelineManager> pipelineManager, std::shared_ptr<DescriptorSetManager> descriptorSetManager);
    ~MaterialManager();

    std::shared_ptr<Material> getMaterial(const std::vector<VertexAttributeDescription>& descriptions, const std::vector<VkDescriptorType>& types, std::vector<std::shared_ptr<Texture>>& textures);

    // TODO: Remove all destroys
    void destroy();

private:
    vkb::Device _device;

    std::vector<std::shared_ptr<Material>> _materials;

    std::shared_ptr<PipelineManager> _pipelineManager;
    std::shared_ptr<DescriptorSetManager> _descriptorSetManager;
};
