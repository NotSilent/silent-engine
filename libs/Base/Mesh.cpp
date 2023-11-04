#include "Mesh.h"

Mesh::Mesh(uint32_t indexCount, VkBuffer indexBuffer, VkBuffer vertexBuffer, VkBuffer attributesBuffer)
        : indexCount(indexCount)
        , indexBuffer(indexBuffer)
        , positionsBuffer(vertexBuffer)
        , attributesBuffer(attributesBuffer) {
}

uint32_t Mesh::getIndexCount() const {
    return indexCount;
}

VkBuffer Mesh::getIndexBuffer() const {
    return indexBuffer;
}

VkBuffer Mesh::getPositionsBuffer() const {
    return positionsBuffer;
}

VkBuffer Mesh::getAttributesBuffer() const {
    return attributesBuffer;
}
