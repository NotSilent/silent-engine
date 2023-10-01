#include "MeshComponent.h"

#include <utility>

void MeshComponent::setMesh(std::shared_ptr<Mesh> newMesh) {
    mesh = std::move(newMesh);
}

void MeshComponent::setPipeline(std::shared_ptr<Pipeline> newPipeline) {
    pipeline = std::move(newPipeline);
}

std::shared_ptr<Mesh> MeshComponent::getMesh() const {
    return mesh;
}

std::shared_ptr<Pipeline> MeshComponent::getPipeline() const {
    return pipeline;
}
