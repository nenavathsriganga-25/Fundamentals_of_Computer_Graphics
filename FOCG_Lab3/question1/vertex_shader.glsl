#version 150 core

in vec3 aPos;
in vec3 aColor;

out vec3 ourColor;

void main()
{
    // Set the position of the vertex
    gl_Position = vec4(aPos, 1.0);

    // Pass the color to the fragment shader
    ourColor = aColor;
}
