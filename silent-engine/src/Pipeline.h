#pragma once
#include "vk-bootstrap\VkBootstrap.h"
#include <memory>

class PipelineLayout;
struct VertexAttribute;
struct VertexAttributeDescription;

// TODO: Add shader dependency
class Pipeline {
public:
    Pipeline(const vkb::Device& device, float width, float height, VkRenderPass renderPass, const std::vector<VertexAttributeDescription>& attributeDescriptions, std::shared_ptr<PipelineLayout> layout);
    ~Pipeline();

    VkPipeline getPipeline() const;
    VkPipelineLayout getPipelineLayout() const;

    bool isCompatible(const std::vector<VertexAttributeDescription>& attributeDescriptions, std::shared_ptr<PipelineLayout> layout);

private:
    vkb::Device _device;

    VkPipeline _pipeline;

    std::vector<VertexAttributeDescription> _attributeDescriptions;
    std::shared_ptr<PipelineLayout> _layout;

    // TODO: Shaders Manager?
    std::tuple<size_t, std::vector<char>> getShaderDataFromFile(const std::string& shaderPath);
    VkShaderModule createShaderModule(const vkb::Device& device, const std::string& shaderFilename);
    VkPipelineShaderStageCreateInfo createPipelineShaderStageCreateinfo(const VkShaderStageFlagBits shaderStage, const VkShaderModule module);
};
