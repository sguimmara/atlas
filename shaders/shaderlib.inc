#version 450
#extension GL_ARB_separate_shader_objects : enable

float light(vec3 source, vec3 normal)
{
    return dot(source.xyz, normal);
}

vec4 transform(mat4 model, mat4 view, mat4 proj, vec3 v)
{
    return (proj * view * model * vec4(v, 1.0));
}