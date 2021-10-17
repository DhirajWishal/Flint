#version 450

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in uint inColor;

layout (push_constant) uniform PushConstants {
	vec2 scale;
	vec2 translate;
} pushConstants;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 outColor;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
	outUV = inUV;
	gl_Position = vec4(inPos * pushConstants.scale + pushConstants.translate, 0.0, 1.0);

	outColor.r = (inColor & 0x000000ff) >> 0;
	outColor.g = (inColor & 0x0000ff00) >> 8;
	outColor.b = (inColor & 0x00ff0000) >> 16;
	outColor.a = (inColor & 0xff000000) >> 24;

	outColor /= 256.0;
}