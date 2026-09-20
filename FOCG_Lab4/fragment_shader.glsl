#version 150 core

in vec3 vertexColor;
out vec4 FragColor;

uniform vec3 solidColor;
uniform int useVertexColor;

void main() {
    if (useVertexColor == 1)
        FragColor = vec4(vertexColor, 1.0);
    else
        FragColor = vec4(solidColor, 1.0);
}