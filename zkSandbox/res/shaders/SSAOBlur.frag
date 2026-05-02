#version 460 core

uniform sampler2D ssaoInput;

in vec2 TexCoords;

out float FragColor;
void main()
{
    const int blurRange = 2;
    int n = 0;
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));
    float result = 0.0;
    for (int x = -blurRange; x <= blurRange; x++)
    {
        for (int y = -blurRange; y <= blurRange; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoInput, TexCoords + offset).r;
            n++;
        }
    }

    FragColor = result / float(n);
}
