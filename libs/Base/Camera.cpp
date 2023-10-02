#include "Camera.h"

#include <numbers>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(float aspectRatio)
        : _projection{createProjection(aspectRatio, std::numbers::pi / 2.0f, 0.1f, 10000.0f)},
          _displacement(glm::vec3(0.0f, 0.0f, -10.0f)) {
}

void Camera::update(glm::vec3 directionInput, glm::vec2 rotationInput, float deltaTime) {
    //glm::vec2 displacement = directionInput * _displacementPerSecond;
    //glm::vec2 rotation = rotationInput /** _anglePerSecond*/;

    //_rotation += rotation;

    //glm::vec3 direction = glm::rotate(FORWARD, _rotation.x, UP);
    //direction = glm::rotate(direction, _rotation.y, RIGHT);

    //glm::vec3 displacement = glm::rotate(glm::vec3(directionInput, 0.0f), _rotation.x, UP);
    //displacement = glm::rotate(direction, _rotation.y, RIGHT);

    _displacement += directionInput * _displacementPerSecond * deltaTime;

    //glm::mat4 m_rotation = glm::rotate(glm::mat4(1.0f), _rotation.x, -UP);
    //m_rotation = glm::rotate(m_rotation, _rotation.y, RIGHT);

    _transform = glm::translate(glm::mat4(1.0f), _displacement);
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
