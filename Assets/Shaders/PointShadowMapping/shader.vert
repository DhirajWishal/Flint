#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inNormal;

layout (location = 0) out vec4 outPos;
layout (location = 1) out vec3 outLightPos;

layout (binding = 0) uniform UBO 
{
	mat4 model;
	vec3 lightPos;
} ubo;

layout (binding = 1) uniform Camera
{
	mat4 projection;
	mat4 view;
} cam;

layout(push_constant) uniform PushConsts 
{
	mat4 view;
} pushConsts;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main()
{
	gl_Position = cam.projection * pushConsts.view * ubo.model * vec4(inPos, 1.0);

	outPos = vec4(inPos, 1.0f);
	outLightPos = ubo.lightPos.xyz; 
}