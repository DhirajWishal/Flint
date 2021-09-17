#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 _Texture;

layout (location = 0) out vec3 TexCoords;
layout (location = 1) out float LODBias;

layout (binding = 0) uniform UBO 
{
	mat4 model;
    mat4 view;
	mat4 projection;
    float LODBias;
} Ubo;

void main()
{
    TexCoords = aPos;
    LODBias = Ubo.LODBias;
    gl_Position = Ubo.projection * mat4(mat3(Ubo.view))  * Ubo.model * vec4(aPos, 1.0f);
}  