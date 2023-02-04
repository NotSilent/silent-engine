#include "Mesh.h"

#include <utility>

Mesh::Mesh(uint32_t indexCount, VkBuffer vertexBuffer, VkBuffer indexBuffer,
           std::vector<VertexAttribute> attributes)
        : _indexCount(indexCount), _vertexBuffer(vertexBuffer), _indexBuffer(indexBuffer),
          _attributes(std::move(attributes)) {
}

void Mesh::destroy(VkDevice device, VmaAllocator allocator) {
    //_vertexBuffer.destroy(device, allocator);
    //_indexBuffer.destroy(device, allocator);
}

uint32_t Mesh::getIndexCount() const {
    return _indexCount;
}

VkBuffer Mesh::getVertexBuffer() const {
    return _vertexBuffer;
}


VkBuffer Mesh::getIndexBuffer() const {
    return _indexBuffer;
}

std::vector<VertexAttribute> const &Mesh::getAttributes() const {
    return _attributes;
}