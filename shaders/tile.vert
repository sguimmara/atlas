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
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 lightDir;

void main()
{
    gl_Position = push.proj * push.view * push.model * vec4(position, 1.0);
    fragNormal = (push.view * push.model * vec4(normal, 0)).xyz;
    fragUv = vec2(uv);
    fragColor = push.color;
    lightDir = (push.view * push.model * vec4(-1, 0, 0.707, 0.0)).xyz;
}
