#include "ShaderManager.h"
#include "Shader.h"

#include <fstream>
#include <iostream>
#include <format>

ShaderManager::ShaderManager(VkDevice device)
    : device(device)
{

}

void ShaderManager::destroy() {
    for (auto& [_, shader] : shaders) {
        vkDestroyShaderModule(device, shader.vert, nullptr);
        vkDestroyShaderModule(device, shader.frag, nullptr);
    }
}

std::optional<Shader> ShaderManager::getShader(const std::string& shaderName) {
    if(auto it = shaders.find(shaderName); it != shaders.end())
    {
        return it->second;
    }

    const auto vertexText = loadShaderFile(std::format("{}.vert", shaderName));
    const auto fragmentText = loadShaderFile(std::format("{}.frag", shaderName));

    if(vertexText.has_value() && fragmentText.has_value())
    {
        auto vertexShader = compileShader(vertexText.value(), shaderc_shader_kind::shaderc_vertex_shader, shaderName);
        auto fragmentShader = compileShader(fragmentText.value(), shaderc_shader_kind::shaderc_fragment_shader, shaderName);
        if(vertexShader.has_value() && fragmentShader.has_value())
        {
            Shader shader {
                .vert = vertexShader.value(),
                .frag = fragmentShader.value(),
            };

            shaders[shaderName] = shader;

            return shader;
        }
    }

    return {};
}

std::optional<std::string> ShaderManager::loadShaderFile(const std::string &shaderName) const {
    // TODO: filesystem
    static const std::string directoryPath = "shaders";
    std::string filePath = std::format("{}/{}", directoryPath, shaderName);

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file.\n";
        return {};
    }

    std::string fileData((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    file.close();

    return fileData;
}

std::optional<VkShaderModule> ShaderManager::compileShader(const std::string &text, shaderc_shader_kind shaderKind,
                                                           const std::string &shaderName) const {
    const shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(text, shaderKind, shaderName.c_str());
    if(result.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cout << result.GetErrorMessage();
    }

    ptrdiff_t codeSize = result.end() - result.begin();

    const VkShaderModuleCreateInfo createInfo{
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = {},
            .codeSize = codeSize * sizeof(uint32_t),
            .pCode = result.begin(),
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        // TODO: enum to string
        const std::string shaderKindName = shaderKind == shaderc_shader_kind::shaderc_vertex_shader ? "vertex" : "fragment";
        std::cout << std::format("Couldn't create {} shader module: {}\n", shaderName, shaderKindName );
    }

    return shaderModule;
}
