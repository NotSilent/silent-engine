#pragma once

#include "Component.h"

#include "Entity.h"
#include "Mesh.h"
#include <memory>
#include <string>

class MeshComponent : public Component {
public:
    void setMesh(std::shared_ptr<Mesh> newMesh);

    void setPipeline(VkPipeline newPipeline);

    [[nodiscard]] std::shared_ptr<Mesh> getMesh() const;

    [[nodiscard]] VkPipeline getPipeline() const;

private:
    std::shared_ptr<Mesh> mesh;
    VkPipeline pipeline;
};
