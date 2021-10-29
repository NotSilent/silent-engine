#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(push_constant) uniform Push {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
} push;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;

void main() {
    
    normal = mat3(transpose(inverse(push.model))) * inNormal;
    position = vec3(push.model * vec4(inPosition, 1.0));
    gl_Position = push.projection * push.view * vec4(position, 1.0);
}