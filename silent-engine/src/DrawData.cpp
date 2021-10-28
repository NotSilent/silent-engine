#include "DrawData.h"

DrawData::DrawData(const Camera& camera)
{
	_camera = camera;
}

DrawData::~DrawData()
{
}

void DrawData::addDrawCall(std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> texture)
{
	_drawCalls.push_back({mesh, texture});
}

Camera DrawData::getCamera() const
{
    return _camera;
}

std::vector<DrawCall> DrawData::getDrawCalls() const
{
    return _drawCalls;
}
