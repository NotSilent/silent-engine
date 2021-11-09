#pragma once
#include "Buffer.h"
#include "tinygltf\tiny_gltf.h"
#include <vulkan\vulkan.h>

enum class VertexAttributeType {
    Position,
    TexCoord0,
    Normal,
    Tangent,
};

struct VertexAttribute {
    VertexAttributeType type;
    VkFormat format;
    uint32_t stride;

    std::shared_ptr<Buffer> buffer;

    static VertexAttributeType getType(const std::string& name)
    {
        if (name == "POSITION") {
            return VertexAttributeType::Position;
        }
        if (name == "TEXCOORD_0") {
            return VertexAttributeType::TexCoord0;
        }
        if (name == "NORMAL") {
            return VertexAttributeType::Normal;
        }
        if (name == "TANGENT") {
            return VertexAttributeType::Tangent;
        }

        throw std::runtime_error("Couldn't parse gltf attribute name");
    }

    static VkFormat getFormat(int gltfType, int code)
    {
        // TODO: Clean this abomination
        // TINYGLTF_COMPONENT_TYPE_BYTE
        // TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE
        // TINYGLTF_COMPONENT_TYPE_SHORT
        // TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT
        // TINYGLTF_COMPONENT_TYPE_INT
        // TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT
        // TINYGLTF_COMPONENT_TYPE_FLOAT
        // TINYGLTF_COMPONENT_TYPE_DOUBLE
        int elements = getTypeElements(gltfType);
        if (elements == 1) {
            switch (code) {
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                return VK_FORMAT_R8_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                return VK_FORMAT_R8_UNORM;
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                return VK_FORMAT_R16_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return VK_FORMAT_R16_UNORM;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return VK_FORMAT_R32_SINT;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return VK_FORMAT_R32_UINT;
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return VK_FORMAT_R32_SFLOAT;
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                return VK_FORMAT_R64_SFLOAT;
            }
        }
        if (elements == 2) {
            switch (code) {
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                return VK_FORMAT_R8G8_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                return VK_FORMAT_R8G8_UNORM;
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                return VK_FORMAT_R16G16_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return VK_FORMAT_R16G16_UNORM;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return VK_FORMAT_R32G32_SINT;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return VK_FORMAT_R32G32_UINT;
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return VK_FORMAT_R32G32_SFLOAT;
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                return VK_FORMAT_R64G64_SFLOAT;
            }
        }
        if (elements == 3) {
            switch (code) {
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                return VK_FORMAT_R8G8B8_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                return VK_FORMAT_R8G8B8_UNORM;
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                return VK_FORMAT_R16G16B16_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return VK_FORMAT_R16G16B16_UNORM;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return VK_FORMAT_R32G32B32_SINT;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return VK_FORMAT_R32G32B32_UINT;
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                return VK_FORMAT_R64G64B64_SFLOAT;
            }
        }
        if (elements == 4) {
            switch (code) {
            case TINYGLTF_COMPONENT_TYPE_BYTE:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case TINYGLTF_COMPONENT_TYPE_SHORT:
                return VK_FORMAT_R16G16B16A16_SNORM;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                return VK_FORMAT_R16G16B16A16_UNORM;
            case TINYGLTF_COMPONENT_TYPE_INT:
                return VK_FORMAT_R32G32B32A32_SINT;
            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                return VK_FORMAT_R32G32B32A32_UINT;
            case TINYGLTF_COMPONENT_TYPE_FLOAT:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case TINYGLTF_COMPONENT_TYPE_DOUBLE:
                return VK_FORMAT_R64G64B64A64_SFLOAT;
            }
        }

        throw std::runtime_error("Couldn't parse gltf component type");
    }

    static uint32_t getFormatSize(VkFormat format)
    {
        // TODO: From map?
        switch (format) {
        case VK_FORMAT_R32_SFLOAT:
            return 4;
        case VK_FORMAT_R32G32_SFLOAT:
            return 8;
        case VK_FORMAT_R32G32B32_SFLOAT:
            return 12;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return 16;
        default:
            throw std::runtime_error("Couldn't get format size");
        }
    }

    static uint32_t getTypeElements(int type)
    {
        switch (type) {
        case TINYGLTF_TYPE_VEC2:
            return 2;
        case TINYGLTF_TYPE_VEC3:
            return 3;
        case TINYGLTF_TYPE_VEC4:
            return 4;
        case TINYGLTF_TYPE_MAT2:
            return 4;
        case TINYGLTF_TYPE_MAT3:
            return 9;
        case TINYGLTF_TYPE_MAT4:
            return 16;
        case TINYGLTF_TYPE_SCALAR:
            return 1;
        case TINYGLTF_TYPE_VECTOR:
            return 4;
        case TINYGLTF_TYPE_MATRIX:
            return 16;

        default:
            throw std::runtime_error("Couldn't get gltf type elements");
        }
    }
};