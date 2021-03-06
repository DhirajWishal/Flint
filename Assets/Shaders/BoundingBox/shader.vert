#version 450 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec4 inColor;

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
	vec4 inColor;
} Ubo;

layout(binding = 1) uniform Camera {
	mat4 view;
	mat4 proj;
} camera;

layout (location = 0) out vec4 outColor;

void main()
{
	gl_Position = camera.proj * camera.view * Ubo.model * vec4(inPosition, 1.0f);

	if(Ubo.inColor.a > 0.0f)
		outColor = Ubo.inColor;

	outColor = inColor;
}