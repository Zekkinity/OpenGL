#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D u_ScreenTexture;
uniform sampler2D u_BloomTexture;

uniform sampler2D u_ScreenDepth;

uniform bool u_GammaCorrection;
uniform bool u_Kernel;
uniform bool u_HDR;

uniform float u_BloomStrength;

uniform float u_Exposure;

layout(std140, binding = 0) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_ViewPos;
    float u_NearPlane;
    float u_FarPlane;
};

float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * u_NearPlane * u_FarPlane) / (u_FarPlane + u_NearPlane - z * (u_FarPlane - u_NearPlane));
}

vec3 DistanceFog(vec3 color)
{
    // Fog
    float depth = texture(u_ScreenDepth, TexCoords).r;
    depth = LinearizeDepth(depth);

    // NOTE: Expose these values
    float fogStart = 30.0;
    float fogEnd = 100.0;
    float maxFog = 0.6;
    vec3 fogColor = vec3(0.6, 0.7, 0.9);

    // Smooth transition into fog
    float fogFactor = smoothstep(fogStart, fogEnd, depth);

    // Falloff
    fogFactor = pow(fogFactor, 1.8);

    // Noise
    float noise = fract(sin(dot(TexCoords * 100.0, vec2(12.9898, 78.233))) * 43758.5453);
    fogFactor += (noise - 0.5) * 0.05;

    // Clamp to max fog value
    fogFactor = clamp(fogFactor, 0.0, maxFog);

    // // Height fog
    // float heightFactor = clamp((u_ViewPos.y - 2.0) * 0.1, 0.0, 1.0);
    // fogFactor *= (1.0 - heightFactor);

    return mix(color, fogColor, fogFactor);
}

vec3 blur(sampler2D tex, vec2 uv, float strength) {
    vec2 texel = 1.0 / textureSize(tex, 0);

    vec3 result = vec3(0.0);

    result += texture(tex, uv + texel * vec2(-1, -1) * strength).rgb;
    result += texture(tex, uv + texel * vec2(0, -1) * strength).rgb;
    result += texture(tex, uv + texel * vec2(1, -1) * strength).rgb;

    result += texture(tex, uv + texel * vec2(-1, 0) * strength).rgb;
    result += texture(tex, uv).rgb;
    result += texture(tex, uv + texel * vec2(1, 0) * strength).rgb;

    result += texture(tex, uv + texel * vec2(-1, 1) * strength).rgb;
    result += texture(tex, uv + texel * vec2(0, 1) * strength).rgb;
    result += texture(tex, uv + texel * vec2(1, 1) * strength).rgb;

    return result / 9.0;
}

void main() {

    // if (gl_FragCoord.x < 1920 / 2) {
    if (u_Kernel) {
        // Invert
        // FragColor = vec4(vec3(1 - texture(u_ScreenTexture, TexCoords)), 1.0);

        // Gray Scale
        // FragColor = texture(u_ScreenTexture, TexCoords);
        // float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
        // FragColor = vec4(average, average, average, 1.0);

        // Gray scale (better)
        // FragColor = texture(u_ScreenTexture, TexCoords);
        // float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        // FragColor = vec4(average, average, average, 1.0);

        const float offset = 1.0 / 300.0;

        vec2 offsets[9] = vec2[](
                vec2(-offset, offset), // top-left
                vec2(0.0f, offset), // top-center
                vec2(offset, offset), // top-right
                vec2(-offset, 0.0f), // center-left
                vec2(0.0f, 0.0f), // center-center
                vec2(offset, 0.0f), // center-right
                vec2(-offset, -offset), // bottom-left
                vec2(0.0f, -offset), // bottom-center
                vec2(offset, -offset) // bottom-right
            );

        // Sharpen kernel
        float kernel[9] = float[](
                -1, -1, -1,
                -1, 9, -1,
                -1, -1, -1
            );

        // Blur kernel
        // float kernel[9] = float[](
        //         1.0 / 16, 2.0 / 16, 1.0 / 16,
        //         2.0 / 16, 4.0 / 16, 2.0 / 16,
        //         1.0 / 16, 2.0 / 16, 1.0 / 16
        //     );

        // Edge kernel
        // float kernel[9] = float[](
        //         1, 1, 1,
        //         1, -8, 1,
        //         1, 1, 1
        //     );

        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(u_ScreenTexture, TexCoords.st + offsets[i]));
        }
        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i];

        FragColor = vec4(col, 1.0);

        // float depth = texture(u_ScreenTexture, TexCoords).r;
        // FragColor = vec4(vec3(depth), 1.0);
    }
    else {
        float pixelSize = 6.0;

        vec2 pixelCoord = TexCoords * vec2(1920, 1080);

        pixelCoord = floor(pixelCoord / pixelSize) * pixelSize;

        vec2 uv = pixelCoord / vec2(1920, 1080);

        FragColor = texture(u_ScreenTexture, uv);
    }

    // Ignore all code above

    vec3 baseColor = texture(u_ScreenTexture, TexCoords).rgb;
    vec3 bloom = texture(u_BloomTexture, TexCoords).rgb;

    baseColor += bloom * u_BloomStrength;

    vec3 finalColor = baseColor;

    // float dist = abs(TexCoords.y - u_FocusLine);
    // float blurFactor = smoothstep(0.0, u_Falloff, dist);
    // vec3 blurred = blur(u_ScreenTexture, TexCoords, u_BlurStrength);

    // Hardcoded tilt shift shader
    // float dist = abs(TexCoords.y - 0.5);
    // float blurFactor = smoothstep(0.0, 0.2, dist);
    // vec3 blurred = blur(u_ScreenTexture, TexCoords, 2.0);

    // finalColor = mix(finalColor, blurred, blurFactor);

    // HDR
    if (u_HDR) {
        // Reinhard tonemapping
        // color = color / (color + vec3(1.0));

        // Tone mapping
        finalColor = vec3(1.0) - exp(-finalColor * u_Exposure);
    }

    // Fog
    // finalColor = DistanceFog(finalColor);

    // Saturation
    // float luminance = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    // vec3 grayScale = vec3(luminance);
    // finalColor = mix(grayScale, finalColor, 1.0);

    // Gamma Correction
    if (u_GammaCorrection) {
        const float gamma = 2.2;

        finalColor = pow(finalColor, vec3(1.0 / gamma));
    }

    FragColor = vec4(finalColor, 1.0);

    // float debug = depth / 100.0;
    // FragColor = vec4(vec3(debug), 1.0);

    // float depth = texture(u_ScreenDepth, TexCoords).r;
    // FragColor = vec4(vec3(depth), 1.0);
}
