#include "DrawData.h"

#include <utility>

DrawData::DrawData(const std::shared_ptr<Camera>& camera, VkPipelineLayout deferredPipelineLayout)
    : deferredPipelineLayout(deferredPipelineLayout)
    , view(camera->getViewMatrix())
    , projection(camera->getProjectionMatrix())
    , directionalLight(glm::vec3(0.0f, 0.0f, -10.0f), glm::vec3(0.0f, 0.0f, 1.0f)){
}

void DrawData::addDrawCall(std::shared_ptr<Mesh> mesh, VkPipeline pipeline, const glm::mat4 &model) {
    DrawCall drawCall;

    drawCall._mesh = std::move(mesh);

    drawCall.pipeline = pipeline;

    drawCall.model = model;

    _drawCalls.emplace_back(drawCall);
}

std::vector<DrawCall> const &DrawData::getDrawCalls() const {
    return _drawCalls;
}
