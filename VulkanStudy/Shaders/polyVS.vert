#version 450

layout(location = 1) in vec3 vPos;

layout (set = 0, binding = 0) uniform UBO
{
	mat4 model;
	mat4 view;
	mat4 proj;
	vec3 lightPos;
}ubo;

void main()
{
	gl_Position = (ubo.proj * ubo.view * ubo.model) * vec4(vPos, 1.0f);
}