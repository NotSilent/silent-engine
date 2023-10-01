#pragma once

#include "glm/mat4x2.hpp"
#include "glm/vec3.hpp"

// TODO: Change to
// Engine
// x right
// y up
// z forward

// 1/((w/h)tan(theta/2))) 0            0       0
// 0                      tan(theta/2) 0       0
// 0                      0            f/(f-n) -fn/(f-n)
// 0                      0            1       0

// Vulkan
// x right
// y bottom
// z forward
//
// Engine
// x right
// y forward
// z up

class Camera {
public:
    Camera() = delete;

    ~Camera() = default;

    explicit Camera(float aspectRatio);

    Camera(const Camera &other) = delete;

    Camera &operator=(const Camera &other) = delete;

    Camera(Camera &&other) = default;

    Camera &operator=(Camera &&other) = default;

    void update(glm::vec2 directionInput, glm::vec2 rotationInput);

    [[nodiscard]] glm::vec3 getPosition() const;

    [[nodiscard]] glm::mat4 getViewMatrix() const;

    [[nodiscard]] glm::mat4 getProjectionMatrix() const;

private:
    // todo: Vector::FORWARD
    static constexpr glm::vec3 FORWARD{0.0f, 1.0f, 0.0f};
    static constexpr glm::vec3 UP{0.0f, 0.0f, 1.0f};
    static constexpr glm::vec3 RIGHT{1.0f, 0.0f, 0.0f};

    static constexpr glm::mat4 _engineToVulkanCoordinateSpace = {1.0f, 0.0f, 0.0f,
                                                                 0.0f,
                                                                 0.0f, 0.0f, 1.0f,
                                                                 0.0f,
                                                                 0.0f, -1.0f, 0.0f,
                                                                 0.0f,
                                                                 0.0f, 0.0f, 0.0f,
                                                                 1.0f};

    glm::mat4 _transform{1.0f};
    glm::mat4 _projection{};

    glm::vec3 _displacement{0.0f};
    glm::vec2 _rotation{0.0f};

    float _displacementPerSecond{0.05f};
    float _anglePerSecond{0.15f};
    float _horizontalAngle{0.0f};
    float _verticalAngle{0.0f};

    glm::vec3 _currentDirection{0.0f, 0.0f, 1.0f};

    glm::mat4 createProjection(float aspectRatio, float fov, float near, float far);
};
