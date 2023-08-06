#include "Camera.h"

#include <numbers>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(float aspectRatio)
        : _projection{createProjection(aspectRatio, std::numbers::pi / 2.0f, 0.1f, 10000.0f)},
          _displacement(glm::vec3(5.0f, -10.0f, 5.0f)) {
}

void Camera::update(glm::vec2 directionInput, glm::vec2 rotationInput) {
    //glm::vec2 displacement = directionInput * _displacementPerSecond;
    glm::vec2 rotation = rotationInput /** _anglePerSecond*/;

    _rotation += rotation;

    glm::vec3 direction = glm::rotate(FORWARD, _rotation.x, UP);
    direction = glm::rotate(direction, _rotation.y, RIGHT);

    glm::vec3 displacement = glm::rotate(glm::vec3(directionInput, 0.0f), _rotation.x, UP);
    displacement = glm::rotate(direction, _rotation.y, RIGHT);

    _displacement += glm::vec3(directionInput, 0.0f) * _displacementPerSecond;

    glm::mat4 m_rotation = glm::rotate(glm::mat4(1.0f), _rotation.x, -UP);
    m_rotation = glm::rotate(m_rotation, _rotation.y, RIGHT);

    _transform = glm::translate(glm::mat4(1.0f), _displacement);
    //glm::lookAt()

//    glm::vec3 eye = _displacement;
//    glm::vec3 center = _displacement + direction;
//    _transform = glm::lookAt(eye, center, UP);
//
//    _transform = glm::translate(glm::mat4(1.0f), eye);
    //_transform = glm::translate(glm::mat4(1.0f), glm::vec3(_displacement, 1.0f));

//    _horizontalAngle += rotationInput.x;
//    _verticalAngle = glm::clamp(_verticalAngle + rotationInput.y, -90.0f + std::numeric_limits<float>::epsilon(),
//                                90.0f - std::numeric_limits<float>::epsilon());
//
//    _currentDirection = glm::rotateX(FORWARD, _verticalAngle);
//    _currentDirection = glm::rotateY(_currentDirection, _horizontalAngle);
//
//    glm::vec3 movementDelta = _currentDirection * directionInput.y;
//    const glm::vec3 cross = glm::cross(_currentDirection, {0.0f, 1.0f, 0.0f});
//    movementDelta -= cross * directionInput.x;
//    _transform += glm::translate(movementDelta);

//    _transform = glm::translate(_transform, glm::vec3(directionInput.x, directionInput.y, 0.0f));
}

glm::vec3 Camera::getPosition() const {
    return glm::vec3(_transform[3]);
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::mat4{1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, -1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f}
           * glm::inverse(_transform);
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
