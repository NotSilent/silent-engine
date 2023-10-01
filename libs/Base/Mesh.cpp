#include "Mesh.h"

#include <utility>

Mesh::Mesh(uint32_t indexCount, VkBuffer vertexBuffer, VkBuffer indexBuffer)
        : _indexCount(indexCount), _vertexBuffer(vertexBuffer), _indexBuffer(indexBuffer) {
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