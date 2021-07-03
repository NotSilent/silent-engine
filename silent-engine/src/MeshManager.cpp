#include "MeshManager.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <iostream>

#include "Mesh.h"

MeshManager::MeshManager(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool commandPool)
    : _device(device)
    , _allocator(allocator)
    , _commandPool(commandPool)
{
}

void MeshManager::addMesh(const std::string& path)
{
    if (_meshes.contains(path)) {
        return;
    }

    auto importer = Assimp::Importer {};

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenSmoothNormals | aiProcess_FixInfacingNormals);

    if (!scene || !scene->HasMeshes()) {
        throw std::runtime_error(importer.GetErrorString());
    }

    const auto assimpMesh = scene->mMeshes[0];

    std::vector<Vertex> vertices { assimpMesh->mNumVertices };
    for (uint32_t i = 0; i < assimpMesh->mNumVertices; ++i) {
        vertices[i].position.x = assimpMesh->mVertices[i].x;
        vertices[i].position.y = assimpMesh->mVertices[i].y;
        vertices[i].position.z = assimpMesh->mVertices[i].z;
        vertices[i].normal.x = assimpMesh->mNormals[i].x;
        vertices[i].normal.y = assimpMesh->mNormals[i].y;
        vertices[i].normal.z = assimpMesh->mNormals[i].z;
    }

    std::vector<uint32_t> indices(assimpMesh->mNumFaces * 3);
    for (uint32_t i = 0; i < assimpMesh->mNumFaces; ++i) {
        indices[i * 3] = assimpMesh->mFaces[i].mIndices[0];
        indices[i * 3 + 1] = assimpMesh->mFaces[i].mIndices[1];
        indices[i * 3 + 2] = assimpMesh->mFaces[i].mIndices[2];
    }

    uint32_t vertexCount = vertices.size();
    uint32_t indexCount = indices.size();
    uint32_t verticesSize = sizeof(Vertex) * vertexCount;
    uint32_t indicesSize = sizeof(uint32_t) * indexCount;

    Buffer<Vertex> vertexBuffer = Buffer<Vertex>(_device, _allocator, _commandPool, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, verticesSize, vertices.data());
    Buffer<uint32_t> indexBuffer = Buffer<uint32_t>(_device, _allocator, _commandPool, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indicesSize, indices.data());

    _meshes[path] = std::make_shared<Mesh>(vertexCount, vertexBuffer, indexCount, indexBuffer);
}

std::weak_ptr<Mesh> MeshManager::getMesh(const std::string& path)
{
    return _meshes[path];
}

void MeshManager::release()
{
    _meshes.clear();
}
