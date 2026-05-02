#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

uniform vec4 u_Color;

void main()
{
    FragColor = u_Color * vec4(1.0);

    BrightColor = u_Color * vec4(1.0);
}
