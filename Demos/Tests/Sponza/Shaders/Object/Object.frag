#version 450

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 UVMap;

layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 TexCoords;
layout (location = 3) in float bias;

layout (binding = 1) uniform sampler2D image;

void main()
{    
    UVMap = vec4(normalize(inNormal), 1.0f);
    FragColor = texture(image, TexCoords, bias);
}