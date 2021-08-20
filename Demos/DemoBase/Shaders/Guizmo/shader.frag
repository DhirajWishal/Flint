#version 450

layout (binding = 3) uniform sampler2D texSampler;

layout (location = 0) in vec2 inTexture;
layout (location = 0) out vec4 color;

void main() 
{	
	color = texture(texSampler, inTexture);

	if(color.a < 1.0f)
		discard;
}