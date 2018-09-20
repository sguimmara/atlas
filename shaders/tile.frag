@SHADER_LIB@

// layout(binding = 0) uniform sampler2D texSampler;
layout(location = 0) in vec2 fragUv;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 lightDir;
layout(location = 0) out vec4 outColor;

void main()
{
	// vec4 tex = texture(texSampler, fragUv);
    float cosAngle = clamp(dot(fragNormal, lightDir), 0.0, 1.0);
    float diff = max(cosAngle, 0.2);
    // outColor = tex * diff;
    outColor = vec4(diff, diff, diff, 1.0);
}
