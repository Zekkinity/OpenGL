#version 460 core

out vec4 FragColor;

in vec2 uv;

uniform samplerCube u_Skybox;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

void main() {
    vec2 ndc = uv * 2.0 - 1.0;

    vec4 clipPos = vec4(ndc, 1.0, 1.0);
    vec4 view = inverse(u_Proj) * clipPos;
    view = vec4(view.xyz, 0.0);

    vec3 dir = normalize((inverse(u_View) * view).xyz);

    FragColor = texture(u_Skybox, dir);
}
