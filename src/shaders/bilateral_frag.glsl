#version 150

uniform sampler2D texture0;
uniform vec2 invRes = vec2(1.0/1024.0, 1.0/1024.0);
uniform int kernelRadius = 5;
uniform float sigmaS = 1.0;
uniform float sigmaI = 0.1;

in vec2 TexCoord;

out float out_frag0;

float bilateralWeight(float ds2, float di2)
{
	const float sigmaS = 5.0;
	return exp( -(ds2)/(2*sigmaS*sigmaS) - (di2)/(2*sigmaI*sigmaI));
}

void main(void)
{
	float i0 = texture(texture0, TexCoord).r;
	float iTotal = 0;
	float wTotal = 0;

	int x;
	int y;

	// infinite loop bug if using Apples intel HD3000 driver
	// May not be confined to Apple.
	// Need constant stop values for for loops.
	
	const int kr = 10;

	for(y=-kr; y<=kr; y++)
	{
		for(x=-kr; x<=kr; x++)
		{
			vec2 offset = vec2(x,y) * invRes;
			float i = texture(texture0, TexCoord + offset).r;

			float ds2 = dot(offset, offset);
			float di = abs(i - i0);
			float w = bilateralWeight(ds2, di*di);

			iTotal += i * w;
			wTotal += w;
			i++;
	 	}
	}

	out_frag0 = iTotal / wTotal;
}