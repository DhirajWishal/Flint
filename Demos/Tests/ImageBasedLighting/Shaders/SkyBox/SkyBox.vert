#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 _Texture;

layout (location = 0) out vec3 TexCoords;
layout (location = 1) out float LODBias;
layout (location = 2) out float exposure;
layout (location = 3) out float gamma;

layout (binding = 0) uniform UBO 
{
	mat4 model;
    mat4 view;
	mat4 projection;
    float LODBias;
    float exposure;
    float gamma;
} Ubo;

void main()
{
    TexCoords = aPos;
    LODBias = Ubo.LODBias;
    exposure = Ubo.exposure;
    gamma = Ubo.gamma;

    gl_Position = Ubo.projection * mat4(mat3(Ubo.view))  * Ubo.model * vec4(aPos, 1.0f);
}  