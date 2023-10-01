#include "PatchedSphere.h"
#include <algorithm>
#include "glm/gtc/epsilon.hpp"

// TODO: Seems to work for now, verify after changing engine coordinate space
// Had to invert corners for the commented faces

PatchedSphere::PatchedSphere(uint32_t subdivisions) {
    // The one that is in the forward direction
    Face forwardFace {
            .topLeft = {1.0f, 1.0f, 1.0f},
            .topRight = {-1.0f, 1.0f, 1.0f},
            .bottomLeft = {1.0f, -1.0f, 1.0f},
            .bottomRight = {-1.0f, -1.0f, 1.0f},
    };

    // The one that actually faces the screen
//    Face backFace {
//            .topLeft = {1.0f, 1.0f, -1.0f},
//            .topRight = {-1.0f, 1.0f, -1.0f},
//            .bottomLeft = {1.0f, -1.0f, -1.0f},
//            .bottomRight = {-1.0f, -1.0f, -1.0f},
//    };

    Face backFace {
            .topLeft = {-1.0f, 1.0f, -1.0f},
            .topRight = {1.0f, 1.0f, -1.0f},
            .bottomLeft = {-1.0f, -1.0f, -1.0f},
            .bottomRight = {1.0f, -1.0f, -1.0f}
    };

//    Face leftFace {
//            .topLeft = {-1.0f, 1.0f, -1.0f},
//            .topRight = {-1.0f, 1.0f, 1.0f},
//            .bottomLeft = {-1.0f, -1.0f, -1.0f},
//            .bottomRight = {-1.0f, -1.0f, 1.0f},
//    };

    Face leftFace {
            .topLeft = {-1.0f, 1.0f, 1.0f},
            .topRight = {-1.0f, 1.0f, -1.0f},
            .bottomLeft = {-1.0f, -1.0f, 1.0f},
            .bottomRight = {-1.0f, -1.0f, -1.0f},
    };

    Face rightFace {
            .topLeft = {1.0f, 1.0f, -1.0f},
            .topRight = {1.0f, 1.0f, 1.0f},
            .bottomLeft = {1.0f, -1.0f, -1.0f},
            .bottomRight = {1.0f, -1.0f, 1.0f},
    };

    // From center of the cube, look up by rotating around x axis
//    Face topFace {
//            .topLeft = {-1.0f, 1.0f, -1.0f},
//            .topRight = {1.0f, 1.0f, -1.0f},
//            .bottomLeft = {-1.0f, 1.0f, 1.0f},
//            .bottomRight = {1.0f, 1.0f, 1.0f},
//    };

    Face topFace {
            .topLeft = {1.0f, 1.0f, -1.0f},
            .topRight = {-1.0f, 1.0f, -1.0f},
            .bottomLeft = {1.0f, 1.0f, 1.0f},
            .bottomRight = {-1.0f, 1.0f, 1.0f},
    };

    Face bottomFace {
            .topLeft = {-1.0f, -1.0f, -1.0f},
            .topRight = {1.0f, -1.0f, -1.0f},
            .bottomLeft = {-1.0f, -1.0f, 1.0f},
            .bottomRight = {1.0f, -1.0f, 1.0f},
    };

    std::vector<Face> faces {
        forwardFace, backFace, leftFace, rightFace, topFace, bottomFace
    };

    for(uint32_t subdivision = 0; subdivision < subdivisions; ++subdivision) {
        faces = generateSubdividedFaces(faces);
    }

    for(const Face& face : faces) {
        glm::vec3 topLeftVector = glm::normalize(face.topLeft);
        glm::vec3 topRightVector = glm::normalize(face.topRight);
        glm::vec3 bottomLeftVector = glm::normalize(face.bottomLeft);
        glm::vec3 bottomRightVector = glm::normalize(face.bottomRight);

        uint32_t topLeftIndex = getIndexOfVertex(topLeftVector);
        uint32_t topRightIndex = getIndexOfVertex(topRightVector);
        uint32_t bottomLeftIndex = getIndexOfVertex(bottomLeftVector);
        uint32_t bottomRightIndex = getIndexOfVertex(bottomRightVector);

        // First triangle
        {
            indices.push_back(topLeftIndex);
            indices.push_back(topRightIndex);
            indices.push_back(bottomLeftIndex);
        }

        // Second triangle
        {
            indices.push_back(bottomLeftIndex);
            indices.push_back(topRightIndex);
            indices.push_back(bottomRightIndex);
        }
    }
}

std::vector<glm::vec3> PatchedSphere::getVertices() const {
    return vertices;
}

std::vector<int32_t> PatchedSphere::getIndices() const {
    return indices;
}

std::vector<Face> PatchedSphere::generateSubdividedFaces(const std::vector<Face> &faces) const {
    std::vector<Face> newFaces;
    for(const Face& face : faces) {
        glm::vec3 topMidpoint = (face.topLeft + face.topRight) / 2.0f;
        glm::vec3 bottomMidpoint = (face.bottomLeft + face.bottomRight) / 2.0f;
        glm::vec3 leftMidpoint = (face.topLeft + face.bottomLeft) / 2.0f;
        glm::vec3 rightMidpoint = (face.topRight + face.bottomRight) / 2.0f;
        glm::vec3 center = (face.topLeft + face.bottomRight) / 2.0f;

        Face topLeftFace {
                .topLeft = face.topLeft,
                .topRight = topMidpoint,
                .bottomLeft = leftMidpoint,
                .bottomRight = center,
        };

        Face topRightFace {
                .topLeft = topMidpoint,
                .topRight = face.topRight,
                .bottomLeft = center,
                .bottomRight = rightMidpoint,
        };

        Face bottomLeftFace {
                .topLeft = leftMidpoint,
                .topRight = center,
                .bottomLeft = face.bottomLeft,
                .bottomRight = bottomMidpoint,
        };

        Face bottomRightFace {
                .topLeft = center,
                .topRight = rightMidpoint,
                .bottomLeft = bottomMidpoint,
                .bottomRight = face.bottomRight,
        };

        newFaces.push_back(topLeftFace);
        newFaces.push_back(topRightFace);
        newFaces.push_back(bottomLeftFace);
        newFaces.push_back(bottomRightFace);
    }

    return newFaces;
}

uint32_t PatchedSphere::getIndexOfVertex(const glm::vec3 &vertex) {
    auto found = std::find_if(vertices.begin(), vertices.end(), [vertex](const glm::vec3& element) {
        return glm::all(glm::epsilonEqual(vertex, element, 0.0001f));
    });

    if(found != vertices.end())
    {
        glm::vec3 f = *found;

        auto eq = glm::epsilonEqual(vertex, f, 0.0001f);
        auto all = glm::all(eq);

        uint32_t distance = std::distance(vertices.begin(), found);

        return std::distance(vertices.begin(), found);
    }

    uint32_t newIndex = vertices.size();

    vertices.push_back(vertex);

    return newIndex;
}
