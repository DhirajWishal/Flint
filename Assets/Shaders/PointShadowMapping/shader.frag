#version 450

//layout (binding = 2) uniform sampler2D texSampler;

layout (location = 0) out float outFragColor;

layout (location = 0) in vec4 inPos;
layout (location = 1) in vec3 inLightPos;
layout (location = 2) in vec2 inUV;

void main() 
{
    //if(texture(texSampler, inUV).a < 1.0f)
    //    discard;

    vec3 lightVec = inPos.xyz - inLightPos;
    outFragColor = length(lightVec);
}