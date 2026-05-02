#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main() {
    vs_out.FragPos = vec3(u_Model * vec4(aPos, 1.0));
    vs_out.Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = u_Proj * u_View * vec4(vs_out.FragPos, 1.0);
}
