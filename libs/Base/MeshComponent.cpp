#include "MeshComponent.h"

#include <utility>

void MeshComponent::setMesh(std::shared_ptr<Mesh> newMesh) {
    mesh = std::move(newMesh);
}

void MeshComponent::setPipeline(VkPipeline newPipeline) {
    pipeline = newPipeline;
}

std::shared_ptr<Mesh> MeshComponent::getMesh() const {
    return mesh;
}

VkPipeline MeshComponent::getPipeline() const {
    return pipeline;
}
