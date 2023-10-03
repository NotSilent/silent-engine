#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
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

    VkPipelineLayout deferredPipelineLayout;

    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;

private:
    std::vector<DrawCall> _drawCalls;
};
