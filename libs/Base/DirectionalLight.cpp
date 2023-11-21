#include "DirectionalLight.h"

#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

DirectionalLight::DirectionalLight(const glm::vec3 &position, const glm::vec3 &direction)
        : position(position), direction(glm::normalize(direction)) {
    projection = engineToVulkanCoordinateSpace * glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.001f, 20.0f);
}

glm::mat4 DirectionalLight::getView() const {
    return glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 DirectionalLight::getProjection() const {
    return projection;
}
