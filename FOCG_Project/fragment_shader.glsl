
#version 150 core

in vec3 Color;
out vec4 FragColor;

uniform vec3 objectColor;
uniform int useSolidColor;
uniform int useLighting;
uniform float alpha;
uniform float glow;

void main() {
    vec3 baseColor;

    if (useSolidColor == 1) {
        baseColor = objectColor;
    } else {
        baseColor = Color;
    }

    if (useLighting == 1) {
        // Simple face-style shading
        baseColor *= 0.85;
    }

    baseColor += glow * objectColor;

    FragColor = vec4(baseColor, alpha);
}