#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(push_constant) uniform Push {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 lightDirection;
} push;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec3 worldPosition;
layout (location = 1) out vec3 normal;

void main() {
    gl_Position = push.projection * push.view * push.model * vec4(inPosition, 1.0);
    
    normal = inNormal, 1.0;
}