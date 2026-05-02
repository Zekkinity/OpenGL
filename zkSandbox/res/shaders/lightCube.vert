#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

uniform mat4 u_Model;

void main() {
    vs_out.FragPos = vec3(u_Model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(u_Model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
}
