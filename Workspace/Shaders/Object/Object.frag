#version 450

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 UVMap;

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 TexCoords;

layout (binding = 1) uniform sampler2D image;

void main()
{    
    FragColor = texture(image, TexCoords);
    UVMap = vec4(normalize(inNormal), 1.0f);
}