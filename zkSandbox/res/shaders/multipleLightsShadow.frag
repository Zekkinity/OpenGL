#version 460 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;

    sampler2D shadowMap;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float intensity;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D depth;
    float shininess;
};

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_FragPosLightSpace;
in mat3 v_TBN;

// Lights
uniform DirLight u_DirLight;

#define MAX_LIGHTS 4
uniform PointLight u_PointLights[MAX_LIGHTS];

uniform int u_PointLightCount;
uniform int u_DirLightCount;
uniform int u_SpotLightCount;

uniform SpotLight u_SpotLight;

uniform Material u_Material;

uniform vec4 u_Color;
uniform bool u_NormalMapping;

uniform bool u_Blinn;

uniform samplerCube u_ShadowCubeMap0;
uniform samplerCube u_ShadowCubeMap1;
uniform samplerCube u_ShadowCubeMap2;
uniform samplerCube u_ShadowCubeMap3;

uniform float u_FarPlane0;
uniform float u_FarPlane1;
uniform float u_FarPlane2;
uniform float u_FarPlane3;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

uniform sampler2D u_DirShadowMap;
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

    vec4 intensity;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec4 attenuation;
};

layout(std140, binding = 1) uniform Lights {
    int pointLightCount;
    int dirLightCount;
    int spotLightCount;
    int _pad;

    PointLightUBO pointLights[4];
    DirLightUBO dirLights[4];
    SpotLightUBO spotLights[4];
};

vec3 sampleOffsetDirections[20] = vec3[](
        vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
        vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
        vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
        vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
    );

float PointShadowCalculation(PointLight light, vec3 fragPos, vec3 normal, samplerCube shadowMap, float farPlane) {
    vec3 fragToLight = fragPos - light.position;
    float currentDepth = length(fragToLight);

    vec3 lightDir = normalize(light.position - fragPos);
    // float bias = 0.15;
    // float bias = max(0.05 * (1.0 - max(dot(normal, lightDir), 0.0)), 0.005);
    // float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.002);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    bias *= currentDepth / farPlane;

    float shadow = 0.0;
    // float samples = 20.0;
    float samples = 12.0;

    float viewDistance = length(u_ViewPos.xyz - fragPos);
    // float diskRadius = 0.05;
    // float diskRadius = (1.0 + (viewDistance / u_FarPlane)) / 25.0;
    // float diskRadius = 0.25 * (viewDistance / farPlane);
    float diskRadius = clamp(0.05 * (viewDistance / farPlane), 0.002, 0.02);

    for (int i = 0; i < samples; i++) {
        vec3 sampleDir = normalize(fragToLight + sampleOffsetDirections[i] * diskRadius);
        float closestDepth = texture(shadowMap, sampleDir).r;
        closestDepth *= farPlane; // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);

    return shadow;

    // For debugging
    // return closestDepth;
}

// Helper to select the right sampler — switch on index, return shadow value directly
float SamplePointShadow(int index, vec3 fragPos, vec3 normal) {
    PointLight light = u_PointLights[index];
    switch (index) {
        case 0:
        return PointShadowCalculation(light, fragPos, normal, u_ShadowCubeMap0, u_FarPlane0);
        case 1:
        return PointShadowCalculation(light, fragPos, normal, u_ShadowCubeMap1, u_FarPlane1);
        case 2:
        return PointShadowCalculation(light, fragPos, normal, u_ShadowCubeMap2, u_FarPlane2);
        case 3:
        return PointShadowCalculation(light, fragPos, normal, u_ShadowCubeMap3, u_FarPlane3);
    }
    return 0.0;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
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

vec3 CalcDirLight(DirLightUBO light, vec3 normal, vec3 viewDir, vec3 texColor, vec4 fragPosLightSpace) {
    vec3 lightDir = normalize(-light.direction.xyz);

    // Ambient
    vec3 ambient = light.ambient.xyz * texColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse.xyz * diff * texColor;

    // Specular
    float spec = 0.0;
    // Blinn-Phong
    if (u_Blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);
    }
    // Phong
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    }

    // Looks weird too shiny
    //     vec3 specMap = texture(u_Material.specular, v_TexCoord).rgb;
    // vec3 specular = light.specular * spec * specMap;
    vec3 specular = light.specular.xyz * spec * texColor;

    float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    return lighting * light.intensity.xyz;

    // return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Ambient
    vec3 ambient = light.ambient * texColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor;

    // Specular
    float spec = 0.0;
    // Blinn-Phong
    if (u_Blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);
    }
    // Phong
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    }

    vec3 specular = light.specular * spec * texColor;

    float shadow = SamplePointShadow(index, fragPos, normal);

    // float shadow = PointShadowCalculation(light, v_FragPos, normal, shadowMap, farPlane);
    // Boost shadow contrast
    // shadow = pow(shadow, 1.5);

    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // float attenuation = 1.0 / distance;

    lighting *= attenuation;

    return lighting * light.intensity;

    // return ambient + diffuse + specular;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(light.position - fragPos);

    // Spot light
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Ambient
    vec3 ambient = light.ambient * texColor;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texColor;

    // Specular
    float spec = 0.0;
    // Blinn-Phong
    if (u_Blinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0), u_Material.shininess);
    }
    // Phong
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    }
    vec3 specular = light.specular * spec * texColor;

    diffuse *= intensity;
    specular *= intensity;

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // FragColor = vec4(vec3(attenuation), 1.0);
    // FragColor = vec4(result, 1.0);
    vec3 lighting = ambient + diffuse + specular;

    return lighting * light.intensity;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    const float heightScale = 0.1;

    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDir), 0.0));

    float layerDepth = 1.0 / numLayers;
    float currentDepth = 0.0;

    vec2 P = viewDir.xy / viewDir.z * heightScale;
    vec2 deltaTexCoords = P / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(u_Material.depth, currentTexCoords).r;

    while (currentDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;

        currentDepthMapValue = texture(u_Material.depth, currentTexCoords).r;

        currentDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth = currentDepthMapValue - currentDepth;
    float beforeDepth = texture(u_Material.depth, prevTexCoords).r - currentDepth + layerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    vec4 texColor = texture(u_Material.diffuse, v_TexCoord) * u_Color;
    if (texColor.a < 0.1)
        discard;

    vec3 normal;
    if (u_NormalMapping) {
        normal = texture(u_Material.normal, v_TexCoord).rgb;
        normal = normal * 2.0 - 1.0;

        // Per fragment matrix multiplication
        normal = normalize(v_TBN * normal);
    } else {
        normal = normalize(v_Normal);
    }

    vec3 viewDir = normalize(u_ViewPos.xyz - v_FragPos);

    vec3 result = vec3(0.0);

    // Directional Lightning UBO
    for (int i = 0; i < dirLightCount; i++) {
        result += CalcDirLight(dirLights[i], normal, viewDir, texColor.rgb, v_FragPosLightSpace);
    }

    // Point lighting
    // for (int i = 0; i < u_PointLightCount; i++) {
    //     result += CalcPointLight(u_PointLights[i], i, normal, v_FragPos, viewDir, texColor.rgb);
    // }

    // Spot lighting
    // for (int i = 0; i < u_SpotLightCount; i++) {
    //     result += CalcSpotLight(u_SpotLight, norm, v_FragPos, viewDir, texColor.rgb);
    // }

    FragColor = vec4(result, 1.0);

    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0);

    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.1)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(vec3(0.0), 1.0);
}
