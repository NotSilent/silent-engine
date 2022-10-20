#include "Mesh.h"

#include <utility>

Mesh::Mesh(uint32_t indexCount, uint32_t byteOffset, uint32_t indexByteSize, std::shared_ptr<Buffer> indexBuffer,
           std::vector<VertexAttribute> attributes)
        : _indexCount(indexCount), _firstIndex(byteOffset / indexByteSize),
          _indexBuffer(std::move(indexBuffer)),
          _attributes(std::move(attributes)) {
}

void Mesh::destroy(VkDevice device, VmaAllocator allocator) {
    //_vertexBuffer.destroy(device, allocator);
    //_indexBuffer.destroy(device, allocator);
}

uint32_t Mesh::getIndexCount() const {
    return _indexCount;
}

VkBuffer Mesh::getIndexBuffer() const {
    return _indexBuffer->getBuffer();
}

std::vector<VertexAttribute> const &Mesh::getAttributes() const {
    return _attributes;
}

uint32_t Mesh::getFirstIndex() const {
    return _firstIndex;
}
