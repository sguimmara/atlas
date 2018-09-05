@SHADER_LIB@

out gl_PerVertex {
    vec4 gl_Position;
};

layout(push_constant) uniform pushConstants {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 color;
} push;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUv;

void main()
{
    vec4 sun = transform(push.model, push.view, push.proj, vec3(1, 0, 0));
    float l = max(0.35, light(normalize(sun.xyz), normalize(normal)));
    gl_Position = transform(push.model, push.view, push.proj, position);
    fragColor = push.color * l;
    fragUv = uv;
}
