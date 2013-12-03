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
	if(value < 1.0e-10)
		return log(1.0e-10)/log(10.0);
		
	return log(value)/log(10.0);
}

float uncharted2Tonemap(float x)
{
	return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float minLogBase = 0.0f;
uniform float maxLogBase = 1.0f;
uniform float targetContrast = 5.0f;
uniform float value = 1.0f;
uniform float exposure = 5.0f;

in vec2 TexCoord;

out vec3 out_frag0;

void main(void)
{
	const float hardExposure = 2.0;

	vec3 color 			= texture(texture0, TexCoord).rgb;
	float logIntensity 	= texture(texture1, TexCoord).r;
	float logBase		= texture(texture2, TexCoord).r;

	float logDetail		= logIntensity - logBase;

	float base 			= pow(10, logBase);

	float whiteScale = 1.0f/uncharted2Tonemap(W);
	float ucBase = uncharted2Tonemap(base * hardExposure * exposure) * whiteScale;
	ucBase = pow(ucBase, 1.0/2.2);

	float logUcBase = log10(ucBase);
	float logUcI = logUcBase + logDetail * value;
	float ucI = pow(10, logUcI);

	float finalIntensity = ucI;

	out_frag0 = color * finalIntensity;
}