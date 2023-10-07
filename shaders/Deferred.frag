#version 450

layout (location = 0) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

void main() {
    //outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    outColor = vec4(inNormal, 1.0f);
}