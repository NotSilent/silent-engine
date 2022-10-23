#pragma once

#include <memory>
#include <vulkan/vulkan_core.h>
#include <vector>

#include "Buffer.h"
#include "Vertex.h"
#include "VertexAttribute.h"

class Mesh {
public:
    Mesh(uint32_t indexCount, uint32_t byteOffset, uint32_t indexByteSize, std::shared_ptr<Buffer> indexBuffer,
         std::vector<VertexAttribute> attributes);

    void destroy(VkDevice device, VmaAllocator allocator);

    [[nodiscard]] uint32_t getIndexCount() const;

    [[nodiscard]] uint32_t getFirstIndex() const;

    [[nodiscard]] VkBuffer getIndexBuffer() const;

    [[nodiscard]] std::vector<VertexAttribute> const &getAttributes() const;

private:
    uint32_t _indexCount;
    uint32_t _firstIndex;
    std::shared_ptr<Buffer> _indexBuffer;

    std::vector<VertexAttribute> _attributes;
};