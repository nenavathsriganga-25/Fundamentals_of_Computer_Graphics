
#version 150 core

in vec3 position;
in vec3 vertexColor;

out vec3 Color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);
    Color = vertexColor;
}