#version 150 core

in vec3 ourColor;

out vec4 FragColor;

void main()
{
    // Set the final pixel color
    FragColor = vec4(ourColor, 1.0);
}
