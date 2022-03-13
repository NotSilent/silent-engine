#version 450
//#extension GL_KHR_vulkan_glsl : enable

layout(push_constant) uniform Push {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
} push;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord0;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec4 tangent;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D colorSampler;
layout(set = 0, binding = 1) uniform sampler2D normalSampler;

vec3 getNormal() {
    vec3 tangentNormal = texture(normalSampler, texCoord0).xyz;

	vec3 q1 = dFdx(position);
	vec3 q2 = dFdy(position);
	vec2 st1 = dFdx(texCoord0);
	vec2 st2 = dFdy(texCoord0);

	vec3 N = normalize(normal);
	vec3 T = normalize(q1 * st2.t - q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

void main() {
    vec3 tangentNormal = getNormal();

    vec3 lightDirection = normalize(vec3(-1.0, 0.5, -1.0));
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec4 objectColor = texture(colorSampler, texCoord0);

    if(objectColor.w < 0.1) {
        discard;
    }

    float ambientValue = 0.1;
    vec3 ambient = lightColor * ambientValue;

    float diffuseValue = max(dot(lightDirection, tangentNormal), 0.0);
    vec3 diffuse = lightColor * diffuseValue;

    vec3 viewDirection = normalize(push.viewPosition - position);
    vec3 reflectDirection = reflect(-lightDirection, tangentNormal);

    float specularValue = 0.5;
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = lightColor * spec * specularValue;

    vec3 color = (ambient + diffuse + specular) * objectColor.xyz;

    outColor = vec4(color, objectColor.w);
}