#include "Mesh.h"

Mesh::Mesh(uint32_t indexCount, vk::Buffer indexBuffer, vk::Buffer vertexBuffer, vk::Buffer attributesBuffer)
        : indexCount(indexCount)
        , indexBuffer(indexBuffer)
        , positionsBuffer(vertexBuffer)
        , attributesBuffer(attributesBuffer) {
}

uint32_t Mesh::getIndexCount() const {
    return indexCount;
}

vk::Buffer Mesh::getIndexBuffer() const {
    return indexBuffer;
}

vk::Buffer Mesh::getPositionsBuffer() const {
    return positionsBuffer;
}

vk::Buffer Mesh::getAttributesBuffer() const {
    return attributesBuffer;
}
