#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTextureCoordinates;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outTexCoordinate;
layout (location = 2) out float bias;

layout (binding = 0) uniform UBO 
{
	mat4 model;
    mat4 view;
	mat4 projection;
    float mipBias;
} Ubo;

void main()
{
    outNormal = inNormal;
    outTexCoordinate = inTextureCoordinates;
    bias = Ubo.mipBias;

    gl_Position = Ubo.projection * Ubo.view * Ubo.model * vec4(inPosition, 1.0f);
}  