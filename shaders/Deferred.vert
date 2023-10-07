#version 450

layout (push_constant) uniform Push {
    mat4 model;
    mat4 view;
    mat4 projection;
} push;

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec3 outNormal;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(push.model)));
    outNormal = normalize(normalMatrix * vec3(inPosition.x, inPosition.y, inPosition.z));

    gl_Position = push.projection * push.view * push.model * vec4(inPosition, 1.0);
}