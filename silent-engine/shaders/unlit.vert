#version 450
#extension GL_KHR_vulkan_glsl : enable

layout(push_constant) uniform Push {
    mat4 model;
    mat4 view;
    mat4 projection;
} push;

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec4 color;

void main() {
    //gl_Position = vec4(inPosition, 1.0);
    gl_Position = push.projection * push.view * push.model * vec4(inPosition, 1.0);
    
    color = vec4(1.0, 0.0, 0.0, 1.0);
    //color = push.projection[0];
}