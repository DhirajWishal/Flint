#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 TexCoords;
layout (location = 1) in float bias;

layout (binding = 1) uniform sampler2D image;

void main()
{    
    FragColor = texture(image, TexCoords, bias);

    if (FragColor.a < 1.0f) 
        discard;
}