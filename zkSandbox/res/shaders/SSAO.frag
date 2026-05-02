#version 460 core

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
};

const vec2 noiseScale = vec2(1920.0 / 4.0, 1080.0 / 4.0);

uniform int u_KernelSize;
uniform float u_Radius;
uniform float u_Bias;

out float FragColor;
void main()
{
    // get data from G-buffer
    vec3 fragPos = vec3(u_View * vec4(texture(gPosition, TexCoords).xyz, 1.0));
    // vec3 normal = normalize(texture(gNormal, TexCoords).xyz);
    vec3 normal = normalize(mat3(transpose(inverse(u_View))) * texture(gNormal, TexCoords).xyz);

    // TRANSFORM DATA TO VIEW SPACE

    // random vector
    ivec2 texDim = textureSize(gPosition, 0);
    ivec2 noiseDim = textureSize(texNoise, 0);
    const vec2 noiseUV = vec2(float(texDim.x)/float(noiseDim.x), float(texDim.y)/float(noiseDim.y)) * TexCoords;
    vec3 randomVec = texture(texNoise, noiseUV).xyz;

    // vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < u_KernelSize; ++i)
    {
        // from tangent to view space
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * u_Radius;

        // from view space to clip space
        vec4 offset = vec4(samplePos, 1.0);
        offset = u_Proj * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5; // [0, 1]

        // vec3 sampleWorld = texture(gPosition, offset.xy).xyz;
        // float sampleDepth = vec3(u_View * vec4(sampleWorld, 1.0)).z;
        float sampleDepth = -texture(gPosition, offset.xy).w;

        float rangeCheck = smoothstep(0.0, 1.0, u_Radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + u_Bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / u_KernelSize);
    FragColor = occlusion;
}
