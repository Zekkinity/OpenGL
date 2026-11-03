#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout(location = 5) in mat4 aModel;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

out mat3 TBN;

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

    mat3 normalMatrix = mat3(transpose(inverse(aModel)));
    Normal = normalize(normalMatrix * aNormal);

    TexCoords = aTexCoords;

    // TBN Matrix
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = Normal;

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
}
