#include "DrawData.h"
#include "Material.h"

DrawData::DrawData(std::shared_ptr<Camera> camera) {
    view = camera->getViewMatrix();
    projection = camera->getProjectionMatrix();
    position = camera->getPosition();
}

DrawData::~DrawData() {
}

void DrawData::addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material, const glm::mat4 &model) {
    DrawCall drawCall;
    for (size_t i = 0; i < mesh->getAttributes().size(); ++i) {
        const VertexAttribute &attrib = mesh->getAttributes()[i];
        drawCall.buffers[i] = attrib.buffer->getBuffer();
        drawCall.offsets[i] = attrib.bufferOffset;
    }

    drawCall.indexBuffer = mesh->getIndexBuffer();
    drawCall.indexCount = mesh->getIndexCount();
    drawCall.firstIndex = mesh->getFirstIndex();

    drawCall.descriptorSet = material->getDescriptorSet();
    drawCall.pipelineLayout = material->getPipelineLayout();
    drawCall.pipeline = material->getPipeline();

    drawCall.model = model;

    _drawCalls.emplace_back(drawCall);
}

//std::shared_ptr<Camera> DrawData::getCamera() const {
//    return std::make_shared<Camera>();
//}

std::vector<DrawCall> const &DrawData::getDrawCalls() const {
    return _drawCalls;
}
