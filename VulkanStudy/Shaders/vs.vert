#version 450

layout(location = 0) in vec3 inPos;
layout(location = 5) in vec3 inNormal;

layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragViewVec;
layout(location = 3) out vec3 fragLightVec;

layout (set = 0, binding = 0) uniform UBO
{
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 lightPos;
}ubo;

void main()
{
	gl_Position = ubo.proj * ubo.view * ubo.model * (vec4(inPos, 1.0) + vec4(6.0f, 7.7f, 0.0f, 1.0f));
	vec4 worldPos = ubo.model * vec4(inPos, 1.0f);
	fragNormal = mat3(ubo.model) * inNormal;
	fragViewVec = (ubo.view * worldPos).xyz;
	fragLightVec = ubo.lightPos - vec3(worldPos);
}