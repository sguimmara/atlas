layout(set = MATERIAL, binding = MATERIAL) uniform MaterialProperties
{
    float shininess;
} material;

float lambert(vec3 normal, vec3 light)
{
    return clamp(dot(-normal, light), 0, 1);
}