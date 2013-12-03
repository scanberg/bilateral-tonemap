#version 150

float log10(float value)
{
	return log(value)/log(10.0);
}

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float exposure = 5.0f;

in vec2 TexCoord;

out vec3 out_frag0;

void main(void)
{
	vec3 color 			= texture(texture0, TexCoord).rgb;
	float logIntensity 	= texture(texture1, TexCoord).r;
	
	float intensity 	= pow(10, logIntensity) * exposure;

	float rhI = intensity / (intensity + 1);

	float finalIntensity = pow(rhI, 1.0/2.2);

	out_frag0 = color * finalIntensity;
}