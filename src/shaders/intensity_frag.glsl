#version 150

uniform sampler2D texture0;
uniform float value = 1.0f;

in vec2 TexCoord;

out vec3 out_frag0;
out float out_frag1;

float log10(float value)
{
	if(value < 1.0e-3)
		return log(1.0e-3)/log(10.0);
		
	return log(value)/log(10.0);
}

void main(void)
{
	vec3 color = texture(texture0, TexCoord).rgb;

	float intensity = dot(color, vec3(20.0, 40.0, 1.0)) / 61.0;
	vec3 rgb = color / intensity;
	float logIntensity = log10(intensity * value);

	out_frag0 = rgb;
	out_frag1 = logIntensity;
}