#version 150

uniform sampler2D texture0;
uniform vec2 invRes = vec2(1.0/1024.0, 1.0/1024.0);
uniform int kernelRadius = 5;
uniform float sigmaS = 1.0;
uniform float sigmaI = 0.1;

in vec2 TexCoord;

out float out_frag0;

float bilateralWeight(float ds, float di)
{
	return exp( -(ds * ds)/(2*sigmaS*sigmaS) - (di * di)/(2*sigmaI*sigmaI));
}

void main(void)
{
	float i0 = texture(texture0, TexCoord).r;
	float iTotal = i0;
	float wTotal = 1;

	const int kr = 10;

	int y = -kr;

	for(; y<0; y++)
	{
		vec2 offset = vec2(0,y) * invRes;
		float i = texture(texture0, TexCoord + offset).r;

		float ds = offset.y;
		float di = abs(i - i0);
		float w = bilateralWeight(ds, di);

		iTotal += i * w;
		wTotal += w;
	}

	for(y=1; y<=kr; y++)
	{
		vec2 offset = vec2(0,y) * invRes;
		float i = texture(texture0, TexCoord + offset).r;

		float ds = offset.y;
		float di = abs(i - i0);
		float w = bilateralWeight(ds, di);

		iTotal += i * w;
		wTotal += w;
	}

	out_frag0 = iTotal / wTotal;
}