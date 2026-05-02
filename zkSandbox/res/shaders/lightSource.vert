// TEMPORAL FILE TO DEBUG LIGHT POOSTIION WITHOUT INSTANCING
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

layout(location = 3) in vec3 tangent;

// NOTE: Can remove this bitangent
layout(location = 4) in vec3 bitangent;

// Instancing
layout(location = 5) in mat4 model;

out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec4 v_FragPosLightSpace;
out mat3 v_TBN;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main() {
    gl_Position = u_Proj * u_View * u_Model * vec4(position, 1.0);
    v_TexCoord = texCoord;

    v_FragPos = vec3(u_Model * vec4(position, 1.0));

    // This is more expensive to do in GPU
    v_Normal = mat3(transpose(inverse(u_Model))) * normal;
    v_FragPosLightSpace = u_LightSpaceMatrix * vec4(v_FragPos, 1.0);

    // TBN Matrix
    vec3 T = normalize(mat3(u_Model) * tangent);
    vec3 N = normalize(mat3(u_Model) * normal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    v_TBN = mat3(T, B, N);
}


