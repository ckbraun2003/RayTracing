#version 330 core

in vec3 vColor;
out vec4 FragColor;

uniform vec2 uResolution;
uniform float uTime;
uniform mat4 view;

uniform vec3 uCameraDir;

struct Ray
{
    vec3 origin;
    vec3 dir;
};


void main()
{
    FragColor = vec4(vColor, 1.0);
}