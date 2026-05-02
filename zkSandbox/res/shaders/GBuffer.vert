#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 5) in mat4 aModel;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

layout(std140, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 viewPos;

    float nearPlane;
    float farPlane;
    float _pad[2];
} camera;

void main() {
    gl_Position = camera.proj * camera.view * aModel * vec4(aPos, 1.0);

    FragPos = vec3(aModel * vec4(aPos, 1.0));

    Normal = mat3(transpose(inverse(aModel))) * aNormal;

    TexCoords = aTexCoords;

}
