#version 460 core

// GBuffer
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
};

uniform Material u_Material;

layout(std140, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 viewPos;

    float nearPlane;
    float farPlane;
    float _pad[2];
} camera;

float linearDepth(float depth)
{
    float z = depth * 2.0f - 1.0f;
    return (2.0f * camera.nearPlane * camera.farPlane) / (camera.farPlane + camera.nearPlane - z * (camera.farPlane - camera.nearPlane));
}

void main()
{
    // Store the fragment position into position buffer
    gPosition = vec4(FragPos, linearDepth(gl_FragCoord.z));

    gNormal = vec4(normalize(Normal), 1.0);

    vec4 albedo = texture(u_Material.diffuse, TexCoords);
    if (albedo.a < 0.1)
        discard;

    gAlbedoSpec.rgb = albedo.rgb;
    gAlbedoSpec.a = texture(u_Material.specular, TexCoords).r;
}
