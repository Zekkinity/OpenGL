#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 a_TexCoord;

out vec2 v_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main() {
    v_TexCoord = a_TexCoord;
    gl_Position = u_LightSpaceMatrix * u_Model * vec4(aPos, 1.0);
}
