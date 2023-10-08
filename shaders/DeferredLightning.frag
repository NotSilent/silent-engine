#version 450

layout (set = 0, binding = 0) uniform sampler2D samplerColor;
layout (set = 0, binding = 1) uniform sampler2D samplerNormal;
layout (set = 0, binding = 2) uniform sampler2D samplerPosition;

layout (location = 0) in vec2 inPos;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(texture(samplerColor, inPos).xyz, 1.0);
    outColor = vec4(texture(samplerNormal, inPos).xyz, 1.0);
    outColor = vec4(texture(samplerPosition, inPos).xyz, 1.0);
}