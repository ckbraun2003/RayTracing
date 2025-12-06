#version 430 core

out vec2 uv;

out vec3 vWorldPosition;

layout (location = 0) in vec3 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec2 uResolution;
uniform float uTime;


void main()
{
    // Multiply the position by the MVP matrix
    gl_Position = projection * view * model * vec4(aPosition, 1.0);

    uv = (gl_Position.xy + 1.0) / 2.0;
    uv.x *= uResolution.x / uResolution.y;

    vWorldPosition = vec3(model * vec4(aPosition, 1.0));

}