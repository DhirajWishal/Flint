#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTextureCoord;

layout (location = 0) out vec2 outTextureCoord;

layout (binding = 0) uniform Uniform 
{
	mat4 model;
} ubo;

layout (binding = 1) uniform Camera 
{
	mat4 view;
	mat4 projection;
} cam;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main()
{
	mat4 modelView = cam.view * ubo.model;

	modelView[0][0] = 1;
	modelView[0][1] = 0;
	modelView[0][2] = 0;
	
	modelView[1][0] = 0;
	modelView[1][1] = 1;
	modelView[1][2] = 0;
	
	modelView[2][0] = 0;
	modelView[2][1] = 0;
	modelView[2][2] = 1;

	gl_Position = cam.projection * modelView * vec4(inPos, 1.0f);
	outTextureCoord = inTextureCoord;
}