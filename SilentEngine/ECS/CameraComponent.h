#pragma once
#include "Component.h"

class Camera;
class InputSystem;

class CameraComponent : public Component {
public:
	CameraComponent();

	virtual void update(float deltaTime) override;

	std::shared_ptr<Camera> getCamera();

private:
	std::shared_ptr<Camera> _camera;
	std::shared_ptr<InputSystem> _inputManager;

	float _displacementPerSecond = 2000.0f;
	float _cameraRotationPerSecond = 2.0f;
};
