#version 450

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inPosition;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outPosition;

void main() {
    outColor = vec4(1, 0, 0, 1.0);
    outNormal = vec4(inNormal, 1.0);
    outPosition = vec4(inPosition, 1.0);

    //outColor = vec4(((inNormal * vec3(1, 1, -1) + 1) / 2), 1.0f);
}