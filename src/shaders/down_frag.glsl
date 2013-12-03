#version 150

uniform sampler2D texture0;
uniform int lod	= 0;

uniform int width 	= 1024;
uniform int height	= 1024;

in vec2 TexCoord;

out vec2 out_frag0;

void main(void)
{
	vec2 minmax = vec2(1.0e20,1.0e-20);

	const int half_kernel = 2;

	int x=-half_kernel;
	int y=-half_kernel;

	ivec2 tc = ivec2(int(TexCoord.x * width), int(TexCoord.y * height));

	vec2 tex;

	for(; x<half_kernel; x++)
	{
		for(; y<half_kernel; y++)
		{
			tex = texelFetch(texture0, tc, lod).rg;
			minmax.x = min(minmax.x, tex.x);
			minmax.y = max(minmax.y, tex.y);
		}
	}

	out_frag0 = minmax;
}