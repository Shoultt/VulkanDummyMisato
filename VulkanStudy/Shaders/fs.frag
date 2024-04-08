#version 450

layout(location = 0) out vec4 outColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragViewVec;
layout(location = 3) in vec3 fragLightVec;

void main()
{
	//outColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	vec3 N = normalize(fragNormal);
	vec3 L = normalize(fragLightVec);
	vec3 V = normalize(fragViewVec);
	vec3 R = reflect(L, N);

	vec3 ambient = vec3(1.0f, 0.0f, 0.0f) * 0.1f;
	vec3 diffuse = max(dot(N, L), 0.0f) * vec3(0.0f, 1.0f, 0.0f);
	vec3 specular = pow(max(dot(R, V), 0.0f), 16.0f) * vec3(1.35f);

	outColor = vec4(ambient + diffuse + specular, 1.0f);
}