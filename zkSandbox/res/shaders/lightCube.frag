#version 460 core

out vec4 FragColor;

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

uniform samplerCube u_ShadowCubeMap;
uniform vec3 u_LightPos;
uniform float u_FarPlane;

vec3 sampleOffsetDirections[20] = vec3[](
        vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
        vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
        vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
        vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
        vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
    );

float ShadowCalculation(vec3 fragPos, vec3 normal) {
    vec3 fragToLight = fragPos - u_LightPos;
    float currentDepth = length(fragToLight);

    vec3 lightDir = normalize(u_LightPos - fragPos);
    // float bias = 0.15;
    float bias = max(0.05 * (1.0 - max(dot(normal, lightDir), 0.0)), 0.005);

    float shadow = 0.0;
    float samples = 20.0;

    float viewDistance = length(u_ViewPos.xyz - fragPos);
    // float diskRadius = 0.05;
    float diskRadius = (1.0 + (viewDistance / u_FarPlane)) / 25.0;

    for (int i = 0; i < samples; i++) {
        float closestDepth = texture(u_ShadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= u_FarPlane; // undo mapping [0;1]
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);

    return shadow;

    // For debugging
    // return closestDepth;
}

void main() {
    vec3 color = texture(u_Material.diffuse, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.5);

    // ambient
    vec3 ambient = 0.3 * lightColor;

    // diffuse
    vec3 lightDir = normalize(u_LightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(u_ViewPos.xyz - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPos, normal);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);

    // FragColor = vec4(vec3(shadow / u_FarPlane), 1.0);
}
