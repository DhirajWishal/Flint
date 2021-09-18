#version 450

layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec3 TexCoords;
layout (location = 1) in float LODBias;
layout (location = 2) in float exposure;
layout (location = 3) in float gamma;

layout (binding = 1) uniform samplerCube skybox;

vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

void main()
{    
    vec3 color = texture(skybox, TexCoords, LODBias).rgb;

	// Tone mapping
	color = Uncharted2Tonemap(color * exposure);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / gamma));
	
	FragColor = vec4(color, 1.0);
}