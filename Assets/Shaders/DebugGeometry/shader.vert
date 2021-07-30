#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} Ubo;

layout(location = 0) in vec3 inPosition;

void main() {
    gl_Position = Ubo.proj * Ubo.view * Ubo.model * vec4(inPosition, 1.0f);
}