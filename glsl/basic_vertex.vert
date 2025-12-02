#version 330 core

layout (location = 0) in vec3 aPosition;

void main()
{
    vec3 position = aPosition * 2.0 - 1.0;
    gl_Position = vec4(position, 1.0);
}