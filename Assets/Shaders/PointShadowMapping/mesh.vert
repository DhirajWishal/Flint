#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inNormal;

layout (binding = 0) uniform UBO 
{
	mat4 model;
} ubo;

layout (binding = 1) uniform Camera
{
	mat4 view;
	mat4 projection;
} cam;

layout (binding = 2) uniform Lights
{
	vec3 lightPos;
} light;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;
layout (location = 2) out vec3 outEyePos;
layout (location = 3) out vec3 outLightVec;
layout (location = 4) out vec3 outWorldPos;
layout (location = 5) out vec3 outLightPos;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main() 
{
	outUV = inUV;
	outNormal = inNormal;
	
	gl_Position = cam.projection * cam.view * ubo.model * vec4(inPos, 1.0);
	outEyePos = vec3(ubo.model * vec4(inPos, 1.0f));
	outLightVec = normalize(light.lightPos - inPos);	
	outWorldPos = inPos;
	
	outLightPos = light.lightPos;	
}

