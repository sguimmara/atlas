#version 450
#extension GL_ARB_separate_shader_objects : enable

out gl_PerVertex {
    vec4 gl_Position;
};

layout(push_constant) uniform pushConstants {
    mat4 mvp;
} pushConsts;

layout(location = 0) out vec3 fragColor;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

void main() {
    gl_Position = pushConsts.mvp * vec4(position, 1.0);
    fragColor = color;
}
