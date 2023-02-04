#pragma once

#include <VkBootstrap.h>
#include <memory>

class PipelineLayout;

struct VertexAttribute;
struct VertexAttributeDescription;

// TODO: Add shader dependency
class Pipeline {
public:
    Pipeline(const vkb::Device &device, float width, float height,
             const std::vector<VertexAttributeDescription> &attributeDescriptions,
             std::shared_ptr<PipelineLayout> layout, const std::string &shaderName);

    ~Pipeline();

    [[nodiscard]] VkPipeline getPipeline() const;

    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

    bool isCompatible(const std::vector<VertexAttributeDescription> &attributeDescriptions,
                      const std::shared_ptr<PipelineLayout> &layout, const std::string &shaderName);

private:
    vkb::Device _device;

    VkPipeline _pipeline;

    std::string _shaderName;

    std::vector<VertexAttributeDescription> _attributeDescriptions;
    std::shared_ptr<PipelineLayout> _layout;

    // TODO: Shaders Manager?
    std::tuple<size_t, std::vector<char>> getShaderDataFromFile(const std::string &shaderPath);

    VkShaderModule createShaderModule(const vkb::Device &device, const std::string &shaderFilename);

    VkPipelineShaderStageCreateInfo
    createPipelineShaderStageCreateinfo(VkShaderStageFlagBits shaderStage, VkShaderModule module);
};
