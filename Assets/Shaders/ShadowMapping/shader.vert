#version 450

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inUV;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec3 inNormal;

layout (binding = 0) uniform Light 
{
	mat4 light;
} light;

out gl_PerVertex 
{
    vec4 gl_Position;   
};

void main()
{
	gl_Position = light.light * vec4(inPos, 1.0);
}