#version 460 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform samplerCube skybox;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

void main() {
    float ratio = 1.00 / 1.52; // Glass

    vec3 I = normalize(Position - u_ViewPos.xyz);
    vec3 R = reflect(I, normalize(Normal));
    // vec3 R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
