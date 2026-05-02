#version 460 core

in vec2 v_TexCoord;

uniform sampler2D u_DiffuseTexture;

void main() {
    if (texture(u_DiffuseTexture, v_TexCoord).a < 0.1) {
        discard;
    }

    // gl_FragDepth = gl_FragCoord.z;
}
