#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} Ubo;

const float mapSize = 1.0f;

void main() {
     gl_Position = Ubo.proj * Ubo.view * Ubo.model * vec4(inPosition, mapSize);

    fragColor = inColor;
}

