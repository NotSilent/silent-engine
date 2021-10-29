#pragma once
#include <glm/mat4x2.hpp>
#include <glm/vec3.hpp>

class Camera {
public:
    Camera(); // delete in the future
    ~Camera();
    Camera(const Camera& other) = default;
    Camera& operator=(const Camera& other);
    Camera(Camera&& other) = default;
    Camera& operator=(Camera&& other) noexcept;

    Camera(float width, float height);

    void update(float deltaTime, glm::vec2 directionInput, glm::vec2 rotationInput);

    glm::vec3 getPosition() const;
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

private:
    const glm::vec3 FORWARD { 0.0f, 0.0f, 1.0f };

    glm::vec3 _position { 0.0f, 0.0f, -1.0f };
    glm::mat4 _projection {};

    float _unitsPerSecond { 1.0f };
    float _angle { 0.15f };
    float _horizontalAngle { 0.0f };
    float _verticalAngle { 0.0f };

    glm::vec3 _currentDirection { 0.0f, 0.0f, 1.0f };
};
