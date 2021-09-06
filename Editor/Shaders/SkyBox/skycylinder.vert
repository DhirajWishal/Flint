#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 _Color;
layout (location = 2) in vec2 aTexture;

layout (location = 0) out vec2 TexCoords;

layout (binding = 0) uniform UBO 
{
	mat4 model;
} Ubo;

layout (binding = 1) uniform Cam
{
    mat4 view;
	mat4 projection;
} Camera;

void main()
{
    TexCoords = aTexture;
    gl_Position = Camera.projection * mat4(mat3(Camera.view))  * Ubo.model * vec4(aPos, 1.0f);
}  