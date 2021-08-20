#version 450 core

layout (location = 0) in vec3 inPosition;

layout(binding = 0) uniform UniformBufferObject {
	mat4 model;
} Ubo;

layout(binding = 1) uniform Camera {
	mat4 view;
	mat4 proj;
} camera;

void main()
{
	gl_Position = camera.proj * camera.view * Ubo.model * vec4(inPosition, 0.05f);
}