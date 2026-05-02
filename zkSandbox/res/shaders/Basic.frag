#version 460 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

out vec4 FragColor;

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texCoords;
} vertex;


layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

uniform Material u_Material;
uniform vec4 u_Color;

void main() {
    vec4 texColor = texture(u_Material.diffuse, vertex.texCoords);

    FragColor = u_Color * texColor;
}
