#version 460 core

// GBuffer
layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
layout(location = 3) out vec4 gMetallicRoughness;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

in mat3 TBN;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D depth;

    sampler2D metallic;
    sampler2D roughness;

    sampler2D metallicRoughness;
    sampler2D ambientOcclusion;
};

uniform Material u_Material;

uniform bool u_NormalMapping;

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

    vec3 normal;
    if (u_NormalMapping)
    {
        normal = texture(u_Material.normal, TexCoords).rgb;
        normal = normal * 2.0 - 1.0;

        normal = normalize(TBN * normal);
    }
    else
    {
        normal = normalize(Normal);
    }
    gNormal = vec4(normal, 1.0);

    vec4 albedo = texture(u_Material.diffuse, TexCoords);

    if (albedo.a < 0.1)
        discard;

    gAlbedoSpec.rgb = albedo.rgb;
    gAlbedoSpec.a = texture(u_Material.specular, TexCoords).r;

    float ao = texture(u_Material.ambientOcclusion, TexCoords).r;

    // Store metallic and roughness values
    float metallic = texture(u_Material.metallic, TexCoords).r;
    float roughness = texture(u_Material.roughness, TexCoords).r;
    gMetallicRoughness = vec4(ao, roughness, metallic, 1.0);

    // vec3 metallicRoughness = texture(u_Material.metallicRoughness, TexCoords).rgb;

    // ORM map
    // gMetallicRoughness = vec4(ao, metallicRoughness.g, metallicRoughness.b, 1.0);
}
