#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec3 TexCoords;
layout (location = 1) in float LODBias;

layout (binding = 1) uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords, LODBias);
}