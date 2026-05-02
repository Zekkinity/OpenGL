#version 460 core

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

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

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;
in vec4 v_FragPosLightSpace;

// Lights
uniform DirLight u_DirLight;

#define MAX_LIGHTS 4
uniform PointLight u_PointLights[MAX_LIGHTS];

uniform int u_PointLightCount;
uniform int u_DirLightCount;
uniform int u_SpotLightCount;

uniform SpotLight u_SpotLight;
//

uniform Material u_Material;

uniform bool u_UseTextures;

uniform bool u_Blinn;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

// float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
//     // perspective divide
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//
//     // Transform from [-1, 1] to [0, 1]
//     projCoords = projCoords * 0.5 + 0.5;
//
//     // check if outside the light projection
//     if (projCoords.z > 1.0) {
//         return 0.0;
//     }
//
//     // get closest depth value from light's perspective
//     float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
//     float currentDepth = projCoords.z;
//
//     float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
//
//     float shadow = 0.0;
//     vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
//     for (int x = -1; x <= 1; ++x) {
//         for (int y = -1; y <= 1; ++y) {
//             float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
//             shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
//         }
//     }
//     shadow /= 9.0;
//
//     // float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
//
//     return shadow;
// }

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 texColor) {
    vec3 lightDir = normalize(-light.direction);

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

    // vec3 specMap = texture(u_Material.specular, v_TexCoord).rgb;
    vec3 specular = light.specular * spec * texColor;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 texColor) {
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

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // float attenuation = 1.0 / distance;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    // float shadow = ShadowCalculation(fragPosLightSpace, normal, lightDir);

    // vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * texColor.rgb;

    // return lighting;

    return ambient + diffuse + specular;
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
    return ambient + diffuse + specular;
}

layout(location = 0) out vec4 FragColor;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
    vec4 texColor = u_UseTextures ? texture(u_Material.diffuse, v_TexCoord) : vec4(1.0);
    if (texColor.a < 0.1)
        discard;

    vec3 norm = normalize(v_Normal);
    vec3 viewDir = normalize(u_ViewPos.xyz - v_FragPos);

    vec3 result = vec3(0.0);

    // Directional lighting
    for (int i = 0; i < u_DirLightCount; i++) {
        result += CalcDirLight(u_DirLight, norm, viewDir, texColor.rgb);
    }

    // Point lighting
    for (int i = 0; i < u_PointLightCount; i++) {
        result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir, texColor.rgb);
    }

    // Spot lighting
    for (int i = 0; i < u_SpotLightCount; i++) {
        result += CalcSpotLight(u_SpotLight, norm, v_FragPos, viewDir, texColor.rgb);
    }

    FragColor = vec4(result, 1.0);

    // float depth = LinearizeDepth(gl_FragCoord.z) / far;
    // FragColor = vec4(vec3(depth), 1.0);
}
