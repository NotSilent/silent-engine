#version 450

layout (location = 0) in vec2 inTextCoord0;
layout (location = 1) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 inTangent;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outPosition;
layout (location = 2) out vec4 outNormal;

layout (set = 0, binding = 0) uniform sampler2D colorSampler;
layout (set = 0, binding = 1) uniform sampler2D normalSampler;

void main()
{
    // TODO: Sample when compositing from material instance?
    vec4 objectColor = texture(colorSampler, inTextCoord0);
    outColor = objectColor;

    // TODO: Change 1.0 to depth?
    outPosition = vec4(inPosition, 1.0f);

    vec3 N = normalize(inNormal);
    vec3 T = normalize(inTangent.xyz);
    T = abs(T);
    //T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    vec3 normal = texture(normalSampler, inTextCoord0).rgb;
    normal = TBN * normalize(normal);

    outNormal = vec4(normal, 1.0);
}