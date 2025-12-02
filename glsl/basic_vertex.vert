#version 330 core

out vec3 vColor;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

uniform mat4 uMVP;

void main()
{
    // Multiply the position by the MVP matrix
    gl_Position = uMVP * vec4(aPosition, 1.0);
    vColor = aColor;
}