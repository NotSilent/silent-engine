#pragma once

#include "glm/glm.hpp"

class DirectionalLight {
public:
    DirectionalLight() = delete;

    DirectionalLight(const glm::vec3& position, const glm::vec3& direction);

    DirectionalLight(const DirectionalLight& other) = delete;
    DirectionalLight& operator=(const DirectionalLight& other) = delete;

    DirectionalLight(DirectionalLight&& other) = default;
    DirectionalLight& operator=(DirectionalLight&& other) = default;

    [[nodiscard]] glm::mat4 getView() const;

    [[nodiscard]] glm::mat4 getProjection() const;

private:
    static constexpr glm::mat4 engineToVulkanCoordinateSpace = {1.0f, 0.0f, 0.0f,0.0f,
                                                                 0.0f, -1.0f, 0.0f, 0.0f,
                                                                 0.0f, 0.0f, 1.0f,0.0f,
                                                                 0.0f, 0.0f, 0.0f, 1.0f};

    glm::vec3 position;
    glm::vec3 direction;

    glm::mat4 projection;
};