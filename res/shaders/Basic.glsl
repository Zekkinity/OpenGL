#shader vertex
#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

out vec3 v_Normal;
out vec2 v_TexCoord;

uniform mat4 u_MVP; // Model View Projection Matrix

void main() {
    gl_Position = u_MVP * vec4(position, 1.0);
    v_TexCoord = texCoord;

    // Transform normal to world space
    v_Normal = mat3(transpose(inverse(u_MVP))) * normal;
}

#shader fragment
#version 460 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

uniform vec4 u_Color;
uniform float u_Steps;

void main() {
    color = u_Color;
}
