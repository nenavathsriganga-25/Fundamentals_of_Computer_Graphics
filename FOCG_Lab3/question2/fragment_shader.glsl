#version 150 core

uniform vec4 ourColor;

out vec4 FragColor;

void main()
{
    // Set the final color of the fragment
    FragColor = ourColor;
}
