#version 460 core

out vec2 uv;

void main() {
    vec2 verts[3] = vec2[](
            vec2(-1.0, -1.0),
            vec2(3.0, -1.0),
            vec2(-1.0, 3.0)
        );

    uv = verts[gl_VertexID] * 0.5 + 0.5;
    gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);
}
