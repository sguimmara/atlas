#version 450
#include "atlas.glsl"
#include "material.glsl"

layout(location = POSITION) in vec3 in_position;
layout(location = ST) in vec2 in_st;
layout(location = NORMAL) in vec3 in_normal;
layout(location = 3) in vec3 in_light;

layout(location = 0) out vec3 fragColor;

void main()
{
    fragColor = material.color.rgb;
}