#pragma once

#include "Buffer.h"
#include <vulkan/vulkan_core.h>

enum class VertexAttributeType {
    Position,
    TexCoord0,
    Normal,
    Tangent,
};

struct VertexAttributeDescription {
    VertexAttributeType type;
    VkFormat format;
    uint32_t stride;

    auto operator<=>(const VertexAttributeDescription &) const = default;
};

struct VertexAttribute {

    VertexAttributeDescription description{};
    VkBuffer buffer;
    VkDeviceSize bufferOffset;

    static VertexAttributeType getType(const std::string &name) {
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

    static uint32_t getFormatSize(VkFormat format) {
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
};