#version 450
#extension GL_KHR_vulkan_glsl : enable

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

layout(set = 0, binding = 0) uniform sampler2D texSampler;

void main() {
    vec3 lightDirection = normalize(vec3(-1.0, 0.5, -1.0));
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColor = texture(texSampler, texCoord0).xyz;

    float ambientValue = 0.1;
    vec3 ambient = lightColor * ambientValue;

    float diffuseValue = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = lightColor * diffuseValue;

    vec3 viewDirection = normalize(push.viewPosition - position);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    float specularValue = 0.5;
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = lightColor * spec * specularValue;

    vec3 color = (ambient + diffuse + specular) * objectColor;

    outColor = vec4(color, 1.0);
}