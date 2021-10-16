#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform CameraObject {
    mat4 view;
    mat4 proj;
} Camera;

layout(location = 0) in vec3 inPosition;
layout(location = 0) out vec3 outPosition;

void main() 
{
    outPosition = inPosition;
    gl_Position =  Camera.proj * Camera.view * vec4(outPosition, 1.0);
}
