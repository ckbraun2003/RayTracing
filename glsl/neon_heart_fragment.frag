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

float dot2(vec2 v)
{
    return dot(v, v);
}

float heart_sdf(vec2 pos)
{
    pos.x = abs(pos.x);
    if( pos.y + pos.x > 1.0 )
    return sqrt(dot2(pos - vec2(0.25,0.75))) - sqrt(2.0) / 4.0;

    return sqrt(min(dot2(pos - vec2(0.00,1.00)), dot2(pos - 0.5 * max(pos.x + pos.y, 0.0)))) * sign(pos.x - pos.y);
}

void main()
{
    float heart_scale = 0.5;
    vec2 pos = (gl_FragCoord.xy * 2.0 - uResolution.xy) / uResolution.y;
    pos.y += 0.25;

    vec3 final_col = vec3(0.0);

    float dist = heart_sdf(pos / heart_scale);
    vec3 col = palette(dist + uTime, a, b, c, d);

    if (dist < 0.0)
    col = vec3(0.0);

    dist = dist * exp(-dist);

    dist = sin(dist * 10.0 + uTime) / 10.0;
    dist = abs(dist);
    dist = 0.02 / dist;


    final_col += col * dist;

    FragColor = vec4(final_col, 1.0);
}