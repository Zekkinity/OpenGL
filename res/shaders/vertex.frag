#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

uniform vec4 u_Color;
uniform float u_Steps;

void main() {
    vec4 texColor = texture(u_Texture, v_TexCoord);
    color = texColor * u_Color;
    // color = texColor;

    // color = u_Color;

    // toon shading
    // vec4 baseColor = u_Color;
    //
    // // Quantize color
    // vec3 toonColor = floor(baseColor.rgb * u_Steps) / u_Steps;
    //
    // color = vec4(toonColor, baseColor.a);
}


