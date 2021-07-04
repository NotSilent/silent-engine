#pragma once

#include <assimp/Importer.hpp>
#include <memory>
#include <string>
#include <unordered_map>

#include "Mesh.h"

class MeshManager {
public:
private:
    vkb::Device _device;
    VmaAllocator _allocator;
    VkCommandPool _commandPool;

    std::unordered_map<std::string, std::shared_ptr<Mesh>> _meshes {};

public:
    MeshManager() = default;

    MeshManager(const vkb::Device& device, const VmaAllocator allocator, const VkCommandPool commandPool);

    void addMesh(const std::string& path);

    std::shared_ptr<Mesh> getMesh(const std::string& path);

    void destroy();

private:
};
