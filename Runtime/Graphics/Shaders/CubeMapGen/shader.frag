#version 450

layout (location = 0) out vec4 FragColor;
layout (location = 0) in vec3 inPosition;

layout (binding = 1) uniform sampler2D skybox;

const vec2 inm_vAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inm_vAtan;
    uv += 0.5;
    return uv;
}

void main()
{    
    vec2 uv = SampleSphericalMap(normalize(inPosition)); 
    vec3 color = texture(skybox, uv).rgb;

    FragColor = vec4(color, 1.0);
}
