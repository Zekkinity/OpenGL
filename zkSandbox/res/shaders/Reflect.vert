#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(location = 5) in mat4 aModel;

out vec3 Normal;
out vec3 Position;
out vec2 TexCoords;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

void main() {
    Normal = mat3(transpose(inverse(aModel))) * aNormal;
    Position = vec3(aModel * vec4(aPos, 1.0));

    gl_Position = u_Proj * u_View * vec4(Position, 1.0);

    TexCoords = aTexCoords;
}
