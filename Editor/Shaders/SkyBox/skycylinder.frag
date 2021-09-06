#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 TexCoords;

layout (binding = 2) uniform sampler2D skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}