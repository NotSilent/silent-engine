#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "DirectionalLight.h"
#include <vector>

struct DrawCall {
    std::shared_ptr<Mesh> _mesh;

    VkPipeline pipeline;

    glm::mat4 model;
};

class DrawData {
public:
    DrawData(const std::shared_ptr<Camera>& camera, VkPipelineLayout deferredPipelineLayout);

    void addDrawCall(std::shared_ptr<Mesh> mesh, VkPipeline pipeline, const glm::mat4 &model);

    [[nodiscard]] std::vector<DrawCall> const &getDrawCalls() const;

    // TODO: Part of material
    VkPipelineLayout deferredPipelineLayout;

    glm::mat4 view;
    glm::mat4 projection;

    DirectionalLight directionalLight;

private:
    std::vector<DrawCall> _drawCalls;
};
