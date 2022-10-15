#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord0;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 inTangent;

layout(push_constant) uniform Push {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
} push;

layout (location = 0) out vec2 outTextCoord0;
layout (location = 1) out vec3 outNormal;

// TODO: Try to save and extract depth from color.a as fragcolor.z

void main()
{
    gl_Position = push.projection * push.view * push.model * vec4(inPosition, 1.0);

    outTextCoord0 = inTexCoord0;
    outNormal = inNormal;
}
