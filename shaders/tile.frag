@SHADER_LIB@

layout(binding = 0) uniform sampler2D texSampler;
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUv;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 lightDir;
layout(location = 0) out vec4 outColor;

void main()
{
	vec4 tex = texture(texSampler, fragUv);
	vec4 col = vec4(fragColor, 1.0);
    float cosAngle = clamp(dot(fragNormal, lightDir), 0.0, 1.0);
    float diff = max(cosAngle, 0.2);
    outColor = col * tex * diff;
}
