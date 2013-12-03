#version 150

float log10(float value)
{
	if(value < 1.0e-3)
		return log(1.0e-3)/log(10.0);
		
	return log(value)/log(10.0);
}

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float minLogBase = 0.0f;
uniform float maxLogBase = 1.0f;
uniform float targetContrast = 5.0f;
uniform float value = 1.0f;

in vec2 TexCoord;

out vec3 out_frag0;

void main(void)
{
	vec3 color 			= texture(texture0, TexCoord).rgb;
	float logIntensity 	= texture(texture1, TexCoord).r;
	float logBase		= texture(texture2, TexCoord).r;

	float logDetail		= logIntensity - logBase;

	float logTarget = log10(targetContrast);

	float compressionFactor = logTarget/(maxLogBase - minLogBase);
	float logAbsScale = maxLogBase * compressionFactor;

	float logOutIntensity = logBase * compressionFactor + logDetail * value;
	
	float intensity 	= pow(10, logIntensity);
	float base 			= pow(10, logBase);
	float outIntensity 	= pow(10, logOutIntensity);

	float finalIntensity = outIntensity;

	out_frag0 = color * finalIntensity;
}