#version 450

layout(push_constant) uniform Push {
    mat4 model;
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
} push;

layout(location = 0) in vec3 worldPosition;
layout(location = 1) in vec3 normal;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDirection = normalize(vec3(-1.0, 0.5, -1.0));
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec3 objectColor = vec3(1.0, 1.0, 1.0);

    float ambientValue = 0.1;
    vec3 ambient = lightColor * ambientValue;

    float diffuseValue = max(dot(lightDirection, normal), 0.0);
    vec3 diffuse = lightColor * diffuseValue;

    vec3 viewDirection = normalize(push.viewPosition - worldPosition);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    float specularValue = 0.5;
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = lightColor * spec * specularValue;

    vec3 color = (ambient + diffuse + specular) * objectColor;

    outColor = vec4(color, 1.0);
}