#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in flat int index;

layout(location = 0) out float outIndex;

void main() {
    vec4 color = texture(texSampler, fragTexCoord);
    if(color.w < 1.0) discard;

    outIndex = index;
    //outColor.rgb *= clamp(dot(vec3(-0.5f, 0.5f, -0.5f), position), 0.1f, 1.0f);
}
