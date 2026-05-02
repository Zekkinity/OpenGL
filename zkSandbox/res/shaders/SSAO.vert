#version 460 core

// NOTE: Can make a fullscreen.vert for all post processing and only change .frag

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0f);
    TexCoords = aTexCoords;
}
