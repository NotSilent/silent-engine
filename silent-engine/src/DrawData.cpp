#include "DrawData.h"

DrawData::DrawData(const Camera& camera)
{
	_camera = camera;
}

DrawData::~DrawData()
{
}

void DrawData::addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture, const glm::mat4& model)
{
	_drawCalls.push_back({
		.mesh = mesh,
		.texture = texture,
		.model = model,
		});
}

Camera DrawData::getCamera() const
{
    return _camera;
}

std::vector<DrawCall> DrawData::getDrawCalls() const
{
    return _drawCalls;
}
