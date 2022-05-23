#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoordinates;

layout (binding = 0) uniform UBO 
{
	mat4 model;
	mat4 view;
	mat4 projection;
} Ubo;

void main()
{
	gl_Position = Ubo.projection * Ubo.view * Ubo.model * vec4(inPosition, 1.0f);
} 