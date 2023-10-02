#include "Camera.h"

#include <numbers>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(float aspectRatio)
        : _projection{createProjection(aspectRatio, std::numbers::pi / 2.0f, 0.1f, 10000.0f)},
          _displacement(glm::vec3(0.0f, 0.0f, -10.0f)) {
}

void Camera::update(glm::vec3 directionInput, glm::vec2 rotationInput, float deltaTime) {

    _displacement += directionInput * _displacementPerSecond * deltaTime;

    // TODO: this is just temp orbiting camera

    _rotation += rotationInput * _anglePerSecond;
    glm::mat4 rotationMat = glm::rotate(glm::mat4(1.0f), _rotation.x, UP);

    _transform = glm::translate(rotationMat, _displacement);
}

glm::vec3 Camera::getPosition() const {
    return _transform[3];
}

glm::mat4 Camera::getViewMatrix() const {
    return _engineToVulkanCoordinateSpace * glm::inverse(_transform);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return _projection;
}

glm::mat4 Camera::createProjection(float aspectRatio, float fov, float near, float far) {
    return {
            1.0f / (aspectRatio * std::tan(fov / 2.0f)),
            0.0f,
            0.0f,
            0.0f,

            0.0f,
            std::tan(fov / 2.0f),
            0.0f,
            0.0f,

            0.0f,
            0.0f,
            far / (far - near),
            1.0f,

            0.0f,
            0.0f,
            -far * near / (far - near),
            0.0f,
    };
}
