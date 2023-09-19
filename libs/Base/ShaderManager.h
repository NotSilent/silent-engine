#pragma once

#include <unordered_map>
#include <string>
#include <vulkan/vulkan_core.h>
#include <optional>
#include <shaderc/shaderc.hpp>
#include "Shader.h"

class ShaderManager {
public:
    explicit ShaderManager(VkDevice device);
    void destroy();

    ShaderManager(ShaderManager& other) = delete;
    ShaderManager& operator=(ShaderManager& other) = delete;

    ShaderManager(ShaderManager&& other) = delete;
    ShaderManager& operator=(ShaderManager&& other) = delete;

    [[nodiscard]] std::optional<Shader> getShader(const std::string& shaderName);

private:
    VkDevice device;
    shaderc::Compiler compiler;

    std::unordered_map<std::string, Shader> shaders;

    [[nodiscard]] std::optional<std::string> loadShaderFile(const std::string& shaderName) const;

    [[nodiscard]] std::optional<VkShaderModule> compileShader(const std::string& text, shaderc_shader_kind shaderKind, const std::string& shaderName) const;
};