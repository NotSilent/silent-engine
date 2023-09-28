#pragma once
#include <memory>
#include "VkBootstrap.h"

// https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#descriptorsets-compatibility
// Two pipeline layouts are defined to be “compatible for push constants”
// if they were created with identical push constant ranges.
// Two pipeline layouts are defined to be “compatible for set N”
// if they were created with identically defined descriptor set layouts for sets zero through N,
// if both of them either were or were not created with VK_PIPELINE_LAYOUT_CREATE_INDEPENDENT_SETS_BIT_EXT,
// and if they were created with identical push constant ranges.

// TODO: refactor
class PipelineLayout {
public:
    PipelineLayout(VkDevice device, uint32_t pushSize);
    ~PipelineLayout();

    [[nodiscard]] VkPipelineLayout getPipelineLayout() const;

private:
    VkDevice device;

    VkPipelineLayout _layout;
};
