#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (binding = 0) uniform UBO 
{
	mat4 projection;
	mat4 view;
	mat4 model;
} ubo;

layout (location = 0) out vec3 outUVW;

out gl_PerVertex 
{
	vec4 gl_Position;
};

mat4 makeStationary(mat4 view) { return mat4(mat3(view)); }

void main() 
{
	outUVW = inPos;
	gl_Position = ubo.projection * makeStationary(ubo.view) * ubo.model * vec4(inPos.xyz, 0.01f);
}