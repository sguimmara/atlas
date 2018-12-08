#version 450
#include "atlas.glsl"

layout(location = ST) in vec2 in_st;
layout(location = NORMAL) in vec3 in_normal;
layout(location = 3) in vec3 in_light;

layout(location = 0) out vec3 fragColor;

void main()
{
    fragColor = vec3(0.7, 0.2, 1);
}