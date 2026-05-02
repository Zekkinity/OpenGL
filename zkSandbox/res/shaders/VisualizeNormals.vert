#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} vs_out;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

uniform mat4 u_Model;

void main()
{
    gl_Position = u_View * u_Model * vec4(aPos, 1.0); 
    mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}
