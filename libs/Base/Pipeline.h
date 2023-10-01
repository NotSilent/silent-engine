#pragma once

#include <VkBootstrap.h>
#include <memory>
#include "Shader.h"

class PipelineLayout;

// TODO: just a handle
class Pipeline {
public:
    // TODO: Per renderpass pipelines, swapchain passed only where relevant
    Pipeline(VkDevice device, float width, float height,
             const std::shared_ptr<PipelineLayout>& layout, VkFormat swapchainFormat, const Shader &shader);

    ~Pipeline();

    [[nodiscard]] VkPipeline getPipeline() const;

    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

    bool isCompatible(const std::shared_ptr<PipelineLayout> &otherLayout);

private:
    VkDevice device;

    VkPipeline pipeline;

    std::shared_ptr<PipelineLayout> layout;

    static VkPipelineShaderStageCreateInfo
    createPipelineShaderStageCreateinfo(VkShaderStageFlagBits shaderStage, VkShaderModule module);
};
