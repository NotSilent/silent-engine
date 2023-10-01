#pragma once

#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"
#include "Pipeline.h"
#include <vector>

struct DrawCall {
    std::shared_ptr<Mesh> _mesh;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    glm::mat4 model;
};

class DrawData {
public:
    explicit DrawData(const std::shared_ptr<Camera>& camera);

    void addDrawCall(std::shared_ptr<Mesh> mesh, const std::shared_ptr<Pipeline>& pipeline, const glm::mat4 &model);

    [[nodiscard]] std::vector<DrawCall> const &getDrawCalls() const;

    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 position;

private:
    std::vector<DrawCall> _drawCalls;
};
