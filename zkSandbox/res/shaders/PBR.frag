#version 460 core

const float PI = 3.14159265359;

// clamp to prevent black spots
vec3 FresnelSchlik(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

layout(std140, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    vec4 position;

    float nearPlane;
    float farPlane;
    float _pad[2];
} camera;

uniform sampler2D u_DirShadowMap;
struct DirLightUBO
{
    vec4 direction;

    vec4 color;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PointLightUBO
{
    vec4 position;

    vec4 color;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 attenuation;
};

struct SpotLightUBO
{
    vec4 position;
    vec4 direction;
    float cutOff;
    float outerCutOff;

    vec4 color;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 attenuation;
};

const int MAX_POINT_LIGHTS = 200;

layout(std140, binding = 1) uniform Lights
{
    int pointLightCount;
    int dirLightCount;
    int spotLightCount;
    int _pad;

    PointLightUBO pointLights[200];
    DirLightUBO dirLights[4];
    SpotLightUBO spotLights[4];
};

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMetallicRoughness;
uniform sampler2D gDepth;
uniform sampler2D ssao;

uniform mat4 u_DirLightMatrix;

uniform float ao;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transform from [-1, 1] to [0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // check if outside the light projection
    if (projCoords.z > 1.0) {
        return 0.0;
    }

    // get closest depth value from light's perspective
    float closestDepth = texture(u_DirShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.002);

    // pcf samples

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_DirShadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_DirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 WorldPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    // vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    vec3 Albedo = pow(texture(gAlbedoSpec, TexCoords).rgb, vec3(2.2)); // linearize

    vec3 orm = texture(gMetallicRoughness, TexCoords).rgb;
    float ambientOcclusion = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    roughness = max(roughness, 0.04);

    vec3 N = normalize(Normal);
    vec3 V = normalize(camera.position.xyz - WorldPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo, metallic);

    vec3 Lo = vec3(0.0);

    // Point lights
    for (int i = 0; i < pointLightCount; ++i)
    {
        PointLightUBO light = pointLights[i];

        vec3 L = normalize(light.position.xyz - WorldPos);
        vec3 H = normalize(L + V);

        float distance = length(light.position.xyz - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.color.xyz * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlik(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    // Directional Lights
    for (int i = 0; i < dirLightCount; ++i)
    {
        DirLightUBO light = dirLights[i];

        vec3 L = normalize(-light.direction.xyz);
        vec3 H = normalize(L + V);

        vec3 radiance = light.color.xyz;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlik(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);

        vec3 lighting = (kD * Albedo / PI + specular) * radiance * NdotL;

        vec4 fragPosLightSpace = u_DirLightMatrix * vec4(WorldPos, 1.0);
        float shadow = ShadowCalculation(fragPosLightSpace, N, L);
        lighting *= (1.0 - shadow);

        Lo += lighting;
    }

    // Spot lights
    for (int i = 0; i < spotLightCount; ++i)
    {
        SpotLightUBO light = spotLights[i];

        vec3 L = normalize(light.position.xyz - WorldPos);
        vec3 H = normalize(L + V);

        float distance = length(light.position.xyz - WorldPos);
        float attenuation = 1.0 / (distance * distance);

        float theta = dot(L, normalize(-light.direction.xyz));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

        vec3 radiance = light.color.xyz * attenuation * intensity;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = FresnelSchlik(max(dot(H, V), 0.0), F0);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * Albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * Albedo * ambientOcclusion;
    vec3 color = ambient + Lo;

    FragColor = vec4(color, 1.0);
}
