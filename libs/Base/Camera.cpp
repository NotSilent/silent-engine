#include "Camera.h"

#include <algorithm>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"

Camera::Camera(float aspectRatio)
        : _projection{glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 10000.0f)} {
}

Camera::Camera() {
}

Camera::~Camera() {
}

Camera &Camera::operator=(const Camera &other) {
    if (this != &other) {
        _position = other._position;
        _projection = other._projection;
        _unitsPerSecond = other._unitsPerSecond;
        _angle = other._angle;
        _horizontalAngle = other._horizontalAngle;
        _verticalAngle = other._verticalAngle;
        _currentDirection = other._currentDirection;
    }

    return *this;
}

Camera &Camera::operator=(Camera &&other) noexcept {
    _position = other._position;
    _projection = other._projection;
    _unitsPerSecond = other._unitsPerSecond;
    _angle = other._angle;
    _horizontalAngle = other._horizontalAngle;
    _verticalAngle = other._verticalAngle;
    _currentDirection = other._currentDirection;

    return *this;
}

void Camera::update(glm::vec2 directionInput, glm::vec2 rotationInput) {
    directionInput *= _unitsPerSecond;
    rotationInput *= _angle;

    _horizontalAngle += rotationInput.x;
    _verticalAngle = glm::clamp(_verticalAngle + rotationInput.y, -90.0f + std::numeric_limits<float>::epsilon(),
                                90.0f - std::numeric_limits<float>::epsilon());

    _currentDirection = glm::rotateX(FORWARD, _verticalAngle);
    _currentDirection = glm::rotateY(_currentDirection, _horizontalAngle);

    glm::vec3 movementDelta = _currentDirection * directionInput.y;
    const glm::vec3 cross = glm::cross(_currentDirection, {0.0f, 1.0f, 0.0f});
    movementDelta -= cross * directionInput.x;
    _position += movementDelta;
}

glm::vec3 Camera::getPosition() const {
    return _position;
}

glm::mat4 Camera::getViewMatrix() const {
    // TODO: up vector has negative y? NDC having y pointing down? Research to be sure
    return glm::lookAt(_position, _position + _currentDirection, {0.0f, -1.0f, 0.0f});
}

glm::mat4 Camera::getProjectionMatrix() const {
    return _projection;
}
