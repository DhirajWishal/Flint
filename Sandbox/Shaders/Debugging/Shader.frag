#version 450

layout (location = 0) in vec2 inTexCoordinates;

layout (location = 0) out vec4 FragColor;

layout (binding = 1) uniform sampler2D image;

void main()
{    
   FragColor = texture(image, inTexCoordinates);
}