#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    int index;
} Ubo;

layout(binding = 1) uniform CameraObject {
    mat4 view;
    mat4 proj;
} Camera;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out int index;

const float mapSize = 1.0f;

void main() {
    gl_Position = Camera.proj * Camera.view * Ubo.model * vec4(inPosition, mapSize);

    fragTexCoord = inTexCoord;
    index = Ubo.index;
}

