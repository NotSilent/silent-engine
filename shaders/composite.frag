#version 450

layout (input_attachment_index = 0, set = 0, binding = 0) uniform subpassInput inputColor;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputPosition;
layout (input_attachment_index = 1, set = 0, binding = 1) uniform subpassInput inputNormal;

layout (location = 0) out vec4 outColor;

void main()
{
    vec3 color = subpassLoad(inputColor).rgb;
    outColor.rgb = color;
}