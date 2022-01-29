#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 position;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
    if(outColor.w < 1.0) discard;


    //outColor.rgb *= clamp(dot(vec3(-0.5f, 0.5f, -0.5f), position), 0.1f, 1.0f);
}
