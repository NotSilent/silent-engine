#pragma once

//#define GLM_FORCE_LEFT_HANDED

#include "glm\glm.hpp"
#include "glm/gtx/transform.hpp"

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

struct PushData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    PushData(float width, float height, float fov, float near, float far)
            : model(glm::mat4(1.0f)), view(createView()), projection(createProjection(width, height, fov, near, far)) {
    }

private:
    glm::mat4 createView() const {
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(5.0f, -10.0f, 5.0f));
        return glm::mat4{1.0f, 0.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 1.0f, 0.0f,
                         0.0f, -1.0f, 0.0f, 0.0f,
                         0.0f, 0.0f, 0.0f, 1.0f}
               * glm::inverse(transform);
    }

    glm::mat4 createProjection(float width, float height, float fov, float near, float far) {
        return {
                1.0f / ((width / height) * std::tan(fov / 2.0f)),
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

};