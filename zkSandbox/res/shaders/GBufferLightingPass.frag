#version 460 core

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gDepth;
uniform sampler2D ssao;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

struct DirLightUBO {
    vec4 direction;

    vec4 intensity;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

struct PointLightUBO {
    vec4 position;

    vec4 intensity;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 attenuation;
};

struct SpotLightUBO {
    vec4 position;
    vec4 direction;
    float cutOff;
    float outerCutOff;

    vec4 intensity;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 attenuation;
};

const int MAX_POINT_LIGHTS = 16;

layout(std140, binding = 1) uniform Lights {
    int pointLightCount;
    int dirLightCount;
    int spotLightCount;
    int _pad;

    PointLightUBO pointLights[MAX_POINT_LIGHTS];
    DirLightUBO dirLights[4];
    SpotLightUBO spotLights[4];
};

vec3 CalcPointLight(PointLightUBO light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specular, float ambientOcclusion)
{
    // Light Volumes
    float distance = length(light.position.xyz - fragPos);
    // if (distance < lights[i].Radius)
    // {

    // ambient
    vec3 ambient = light.ambient.xyz * albedo * ambientOcclusion;

    // diffuse
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * light.diffuse.xyz;

    // Specular
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64);

    vec3 specularL = specular * spec * light.specular.xyz;

    // vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);
    vec3 lighting = (ambient + diffuse + specularL) * light.intensity.xyz;

    // float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);

    lighting *= attenuation;

    return lighting;
}

vec3 CalcDirLight(DirLightUBO light, vec3 normal, vec3 viewDir, vec3 albedo, float specular, float ambientOcclusion)
{
    vec3 lightDir = normalize(-light.direction.xyz);

    // Ambient
    vec3 ambient = light.ambient.xyz * albedo * ambientOcclusion;

    // Diffuse
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * light.diffuse.xyz;

    // Specular
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32);

    vec3 specularL = specular * spec * light.specular.xyz;

    vec3 lighting = (ambient + diffuse + specularL) * light.intensity.xyz;

    return lighting;
}

vec3 CalcSpotLight(SpotLightUBO light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float specular)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Ambient
    vec3 ambient = light.ambient.xyz * albedo;

    // Diffuse
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * albedo * light.diffuse.xyz;

    // Specular
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32);

    vec3 specularL = specular * spec * light.specular.xyz;

    diffuse *= intensity;
    specularL *= intensity;

    vec3 lighting = (ambient + diffuse + specularL) * light.intensity.xyz;

    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distance + light.attenuation.z * distance * distance);

    lighting *= attenuation;

    return lighting;
}

out vec4 FragColor;
void main()
{
    // float depth = texture(gDepth, TexCoords).r;
    //
    // vec4 clip = vec4(TexCoords * 2.0 - 1.0, depth, 1.0);
    // vec4 view = inverse(u_Proj) * clip;
    // view /= view.w;
    //
    // vec3 fragPos = view.xyz;

    // retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    float ambientOcclusion = texture(ssao, TexCoords).r;

    vec3 viewDir = normalize(u_ViewPos.xyz - FragPos);

    // then calculate lighting as usual
    // vec3 lighting = Albedo * 0.1; // hard-coded ambient component
    vec3 lighting = vec3(0.0);

    // Point Lights
    for (int i = 0; i < pointLightCount; ++i)
    {
        const PointLightUBO light = pointLights[i];
        lighting += CalcPointLight(light, Normal, FragPos, viewDir, Albedo, Specular, ambientOcclusion);
    }

    // Directional Lights
    for (int i = 0; i < dirLightCount; ++i)
    {
        const DirLightUBO light = dirLights[i];
        lighting += CalcDirLight(light, Normal, viewDir, Albedo, Specular, ambientOcclusion);
    }

    // Spot Lights
    for (int i = 0; i < spotLightCount; ++i)
    {
        const SpotLightUBO light = spotLights[i];
        lighting += CalcSpotLight(light, Normal, FragPos, viewDir, Albedo, Specular);
    }

    FragColor = vec4(lighting, 1.0);
}
