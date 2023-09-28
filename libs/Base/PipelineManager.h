#pragma once

#include "VkBootstrap.h"
#include "ShaderManager.h"
#include "VertexAttribute.h"
#include "PipelineLayout.h"
#include <memory>
#include <optional>

class Pipeline;

class PipelineLayoutManager;

class PipelineManager {
public:
    PipelineManager(VkDevice device, float width, float height,
                    std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager);

    std::optional<std::shared_ptr<Pipeline>> getPipeline(const std::vector<VertexAttributeDescription> &descriptions,
                                          const std::vector<VkDescriptorType> &types, const std::string &shaderName);

    // TODO: Remove all destroys
    void destroy();

private:
    VkDevice device;

    float width;
    float height;

    std::vector<std::shared_ptr<Pipeline>> pipelines;
    std::shared_ptr<PipelineLayoutManager> pipelineLayoutManager;

    ShaderManager shaderManager;
};

// https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#descriptorsets-compatibility
// Two pipeline layouts are defined to be “compatible for push constants”
// if they were created with identical push constant ranges.
// Two pipeline layouts are defined to be “compatible for set N”
// if they were created with identically defined descriptor set layouts for sets zero through N,
// if both of them either were or were not created with VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT,
// and if they were created with identical push constant ranges.

// Abstract descriptors
// 0 -> Per Frame
// 1 -> Per RenderPass
// 2 -> Per Material
// 3 -> Per Object