#version 460 core

in vec4 FragPos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main() {
    // Distance from light to fragment
    float lightDistance = length(FragPos.xyz - u_LightPos);

    // map to [0, 1]
    lightDistance = lightDistance / u_FarPlane;

    gl_FragDepth = lightDistance;
}
