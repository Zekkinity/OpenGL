#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

// Instancing
layout(location = 5) in mat4 model;

// Non-instancing
// uniform mat4 model;
uniform mat4 u_Model;

out VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

void main() {
    // This is more expensive to do in GPU
    vertex.normal = mat3(transpose(inverse(model))) * aNormal;
    vertex.position = vec3(model * vec4(aPos, 1.0));

    gl_Position = u_Proj * u_View * vec4(vertex.position, 1.0);

    vertex.texCoords = aTexCoords;
}
