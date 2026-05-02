#version 460 core

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

void main() {
    TexCoords = aPos;
    vec4 pos = u_Proj * mat4(mat3(u_View)) * vec4(aPos, 1.0);
    // vec4 pos = u_Proj * u_View * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
