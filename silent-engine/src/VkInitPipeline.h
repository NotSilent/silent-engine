#pragma once
#include <string>
#include <vulkan\vulkan.h>
#include <vk-bootstrap\VkBootstrap.h>

// TODO: Extract shader creation, shader manager?

namespace VkInit::Pipeline {
std::tuple<size_t, std::vector<char>> getShaderDataFromFile(const std::string& shaderPath);

VkShaderModule createShaderModule(const vkb::Device& device, const std::string& shaderFilename);

VkPipelineShaderStageCreateInfo createPipelineShaderStageCreateinfo(const VkShaderStageFlagBits shaderStage, const VkShaderModule module);

VkPipelineLayout createPipelineLayout(const vkb::Device& device, const uint32_t setLayoutCount, const VkDescriptorSetLayout* setLayouts, uint32_t pushSize);

VkPipeline createDefaultPipeline(const vkb::Device& device, const VkPipelineLayout layout, const VkRenderPass renderPass, const uint32_t width, const uint32_t height);
}