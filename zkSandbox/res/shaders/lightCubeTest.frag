#version 460 core

out vec4 FragColor;

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

uniform Material u_Material;

#define MAX_LIGHTS 4
uniform int u_PointLightCount;
uniform PointLight u_PointLights[MAX_LIGHTS];

uniform samplerCube u_ShadowCubeMaps[MAX_LIGHTS];
uniform float u_FarPlanes[MAX_LIGHTS];

uniform bool u_Blinn;

vec3 sampleOffsetDirections[20] = vec3[](
        vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
        vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
        vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
        vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
    );

float ShadowCalculation(int i, vec3 fragPos, vec3 normal)
{
    vec3 fragToLight = fragPos - u_PointLights[i].position;
    float currentDepth = length(fragToLight);

    vec3 lightDir = normalize(u_PointLights[i].position - fragPos);
    float bias = max(0.05 * (1.0 - max(dot(normal, lightDir), 0.0)), 0.005);

    float shadow = 0.0;
    float samples = 20.0;

    float viewDistance = length(u_ViewPos.xyz - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_FarPlanes[i])) / 25.0;

    for (int s = 0; s < samples; s++) {
        float closestDepth =
            texture(u_ShadowCubeMaps[i],
                    fragToLight + sampleOffsetDirections[s] * diskRadius).r;

        closestDepth *= u_FarPlanes[i];

        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    return shadow / samples;
}

vec3 CalcPointLight(int i, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    PointLight light = u_PointLights[i];

    vec3 texColor = texture(u_Material.diffuse, fs_in.TexCoords).rgb;

    vec3 lightDir = normalize(light.position - fragPos);

    // Ambient
    vec3 ambient = light.ambient * texColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor;

    // Specular
    float spec = 0.0;
    if (u_Blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);
    } else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    }

    vec3 specular = light.specular * spec * texColor;

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 /
        (light.constant + light.linear * distance +
         light.quadratic * distance * distance);

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    float shadow = ShadowCalculation(i, fragPos, normal);

    return ambient + (1.0 - shadow) * (diffuse + specular);
}

void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(u_ViewPos.xyz - fs_in.FragPos);

    vec3 result = vec3(0.0);

    for (int i = 0; i < u_PointLightCount; i++) {
        result += CalcPointLight(i, normal, fs_in.FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0);
}
