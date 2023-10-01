#pragma once

#include "Component.h"

#include "Entity.h"
#include "Mesh.h"
#include <memory>
#include <string>

class Pipeline;

class MeshComponent : public Component {
public:
    void setMesh(std::shared_ptr<Mesh> newMesh);

    void setPipeline(std::shared_ptr<Pipeline> newPipeline);

    [[nodiscard]] std::shared_ptr<Mesh> getMesh() const;

    [[nodiscard]] std::shared_ptr<Pipeline> getPipeline() const;

private:
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Pipeline> pipeline;
};
