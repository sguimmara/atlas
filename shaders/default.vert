#version 450
#include "atlas.glsl"

layout(location = POSITION) in vec3 in_position;
layout(location = NORMAL) in vec3 in_normal;
layout(location = ST) in vec2 in_st;

layout(location = ST) out vec2 out_st;
layout(location = NORMAL) out vec3 out_normal;
layout(location = 3) out vec3 out_light;

void main()
{
    out_normal = transformToViewSpace(vec4(in_normal, 0));
    out_light = transformToViewSpace(globals.sunDirection);
    out_st = in_st;

    gl_Position = atl_transformPos(in_position);
}
