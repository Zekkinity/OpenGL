#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

uniform vec3 u_ObjectColor;
uniform vec3 u_LightColor;

void main() {
    // vec4 texColor = texture(u_Texture, v_TexCoord);
    FragColor = vec4(u_ObjectColor, 1.0);
    FragColor = vec4(1, 0, 1, 1);
}
