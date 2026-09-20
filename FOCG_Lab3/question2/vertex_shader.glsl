#version 150 core

in vec3 aPos;

void main()
{
    // Set the vertex position
    gl_Position = vec4(aPos, 1.0);
}
