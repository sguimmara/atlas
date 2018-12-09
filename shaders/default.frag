#version 450
#include "atlas.glsl"
#include "material.glsl"

layout(set = MATERIAL, binding = 3) uniform sampler2D diffuse;
layout(set = MATERIAL, binding = 4) uniform sampler2D specular;

layout(location = POSITION) in vec3 in_position;
layout(location = ST) in vec2 in_st;
layout(location = NORMAL) in vec3 in_normal;
layout(location = 3) in vec3 in_light;

layout(location = 0) out vec3 fragColor;

void main()
{
    vec3 color = texture(diffuse, in_st).rgb;
    vec3 specular = texture(specular, in_st).rgb;

    float diff = clamp(lambert(in_normal, in_light), 0.1, 1);
    vec3 lightColor = globals.sunColorAndIntensity.xyz;
    float lightIntensity = globals.sunColorAndIntensity.w;

    vec3 finalColor = color * lightColor * lightIntensity * diff;

    fragColor = finalColor;
}