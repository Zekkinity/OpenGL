#version 460 core

layout(location = 0) out vec4 color;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_LightDir;   // directional light
uniform vec3 u_LightColor;
uniform vec3 u_ViewPos;

uniform vec4 u_Color;

void main()
{
    vec3 normal = normalize(v_Normal);
    vec3 lightDir = normalize(-u_LightDir);

    // Ambient
    vec3 ambient = 0.15 * u_Color.rgb;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * u_Color.rgb * u_LightColor;

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    vec3 specular = spec * u_LightColor * 0.3;

    vec3 result = ambient + diffuse + specular;
    color = vec4(result, u_Color.a);
}

