#version 150

const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float W = 11.2;

float log10(float value)
{
	return log(value)/log(10.0);
}

float uncharted2Tonemap(float x)
{
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float exposure = 5.0f;

in vec2 TexCoord;

out vec3 out_frag0;

void main(void)
{
	const float hardExposure = 2.0;
	vec3 color 			= texture(texture0, TexCoord).rgb;
	float logIntensity 	= texture(texture1, TexCoord).r;
	float intensity 	= pow(10, logIntensity) * exposure * hardExposure;

	float whiteScale = 1.0f/uncharted2Tonemap(W);

	float ucI = uncharted2Tonemap(intensity) * whiteScale;

	float finalIntensity = pow(ucI, 1.0/2.2);

	out_frag0 = color * finalIntensity;
}