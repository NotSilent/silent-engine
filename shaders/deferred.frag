#version 450

layout (location = 0) in vec2 inTextCoord0;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outPosition;
layout (location = 2) out vec4 outNormal;

layout(set = 0, binding = 0) uniform sampler2D colorSampler;
layout(set = 0, binding = 1) uniform sampler2D normalSampler;

void main()
{
    // TODO: Sample when compositing from material instance?
    vec4 objectColor = texture(colorSampler, inTextCoord0);
    outColor = objectColor;
    outPosition = vec4(1.0, 0.0, 0.0, 1.0);
    outNormal = vec4(inNormal, 1.0);
}