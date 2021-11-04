#version 330 core
uniform vec3 LampColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(LampColor, 1.0); // set all 4 vector values to 1.0
}