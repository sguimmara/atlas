#version 450
#extension GL_ARB_separate_shader_objects : enable

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

void main() {
	vec4 sun = push.proj * push.view * push.model * vec4(1, 0, 0, 1.0);
	float light = dot(sun.xyz, normal);
    gl_Position = push.proj * push.view * push.model * vec4(position, 1.0);
    fragColor = push.color * light;
}
