#version 330 core

out vec4 FragColor;

uniform vec2 uResolution;
uniform float uTime;

vec3 a = vec3(0.5, 0.5, 0.5);
vec3 b = vec3(0.5, 0.5, 0.5);
vec3 c = vec3(1.0, 1.0, 1.0);
vec3 d = vec3(0.263, 0.416, 0.557);

vec3 palette(float t, vec3 a, vec3 b, vec3 c, vec3 d)
{
    return a + b * cos(6.28318 * (c * t * d));
}

void main()
{
    vec2 uv = (gl_FragCoord.xy * 2.0 - uResolution.xy) / uResolution.y;
    vec2 uv_0 = uv;
    vec3 final_col = vec3(0.0);

    for (float i = 0.0; i < 4.0; ++i)
    {
        uv = fract(uv * 1.5) - 0.5;

        float dist = length(uv) * exp(-length(uv_0));

        vec3 col = palette(length(uv_0) + uTime, a, b, c, d);

        dist = sin(dist * 8.0 + uTime) / 8.0;
        dist = abs(dist);
        dist = 0.01 / dist;

        final_col += col * dist;
    }

    FragColor = vec4(final_col, 1.0);
}