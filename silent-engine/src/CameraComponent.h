#pragma once
#include "Component.h"

class Camera;
class InputManager;

class CameraComponent : public Component {
public:
	CameraComponent();

	virtual void update(float deltaTime) override;

	std::shared_ptr<Camera> getCamera();

private:
	std::shared_ptr<Camera> _camera;
	std::shared_ptr<InputManager> _inputManager;

	float _displacementPerSecond = 1000.0f;
	float _cameraRotationPerSecond = 10000.0f;
};
