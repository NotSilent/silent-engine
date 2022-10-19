#version 450

layout (location = 0) in vec2 inTextCoord0;
layout (location = 1) in vec3 inPosition;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec4 inTangent;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 outPosition;
layout (location = 2) out vec4 outNormal;

layout(set = 0, binding = 0) uniform sampler2D colorSampler;
layout(set = 0, binding = 1) uniform sampler2D normalSampler;

//vec3 getNormal() {
//    vec3 tangentNormal = texture(normalSampler, inTextCoord0).xyz;
//
//    vec3 q1 = dFdx(inPosition);
//    vec3 q2 = dFdy(inPosition);
//    vec2 st1 = dFdx(inTextCoord0);
//    vec2 st2 = dFdy(inTextCoord0);
//
//    vec3 N = normalize(inNormal);
//    vec3 T = normalize(q1 * st2.t - q2 * st1.t);
//    vec3 B = -normalize(cross(N, T));
//    mat3 TBN = mat3(T, B, N);
//
//    return normalize(TBN * tangentNormal);
//}

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
    vec3 B = normalize(normalize(cross(N, T)));
    mat3 TBN = mat3(T, B, N);

    vec3 normal = texture(normalSampler, inTextCoord0).rgb;
    normal = normalize(TBN * normalize(normal));

    outNormal = vec4(normal, 1.0);
}