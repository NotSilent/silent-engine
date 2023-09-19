#pragma once

#include <VkBootstrap.h>
#include <memory>
#include "Shader.h"

class PipelineLayout;

struct VertexAttribute;
struct VertexAttributeDescription;

// TODO: just a handle
class Pipeline {
public:
    Pipeline(VkDevice device, float width, float height,
             const std::vector<VertexAttributeDescription> &attributeDescriptions,
             const std::shared_ptr<PipelineLayout>& layout, const Shader &shader);

    ~Pipeline();

    [[nodiscard]] VkPipeline getPipeline() const;

    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

    bool isCompatible(const std::vector<VertexAttributeDescription> &otherAttributeDescriptions,
                      const std::shared_ptr<PipelineLayout> &otherLayout);

private:
    VkDevice device;

    VkPipeline pipeline;

    std::vector<VertexAttributeDescription> attributeDescriptions;
    std::shared_ptr<PipelineLayout> layout;

    static VkPipelineShaderStageCreateInfo
    createPipelineShaderStageCreateinfo(VkShaderStageFlagBits shaderStage, VkShaderModule module);
};
