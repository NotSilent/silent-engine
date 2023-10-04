#version 450

layout (set = 0, binding = 0) uniform sampler2D samplerColor;

layout (location = 0) in vec2 inPos;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(texture(samplerColor, inPos).xyz, 1.0);
}