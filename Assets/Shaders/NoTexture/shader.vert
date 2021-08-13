#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTextureCoord;

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
	gl_Position = cam.projection * cam.view * ubo.model * vec4(inPos, 1.0f);
}