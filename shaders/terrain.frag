#version 450
#include "atlas.glsl"
#include "material.glsl"

layout(set = MATERIAL, binding = 3) uniform sampler2D image;

layout(location = POSITION) in vec3 in_position;
layout(location = ST) in vec2 in_st;
layout(location = NORMAL) in vec3 in_normal;
layout(location = 3) in vec3 in_light;

layout(location = 0) out vec3 fragColor;

void main()
{
    vec3 color = texture(image, in_st).rgb;

    float lambertian = clamp(lambert(in_normal, in_light), 0.1, 1);
    vec3 reflectDir = reflect(-in_light, in_normal);
    float specAngle = max(dot(reflectDir, normalize(vec3(-2, 2, 0))), 0.0);
    float specular = clamp(pow(specAngle, 64.0), 0, 1);

    vec3 water = vec3(0.008, 0.02, 0.078);
    float d = 1 - (clamp(10 * distance(water, color), 0, 1));

    float finalSpecular = specular * d * lambertian;
    vec3 lightColor = globals.sunColorAndIntensity.xyz;
    float lightIntensity = globals.sunColorAndIntensity.w;

    fragColor = (color * lambertian * lightIntensity) +
    (lightIntensity * finalSpecular * lightColor);
}