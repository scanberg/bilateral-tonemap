#version 150

uniform sampler2D texture0;
uniform int lod	= 0;
uniform int width 	= 1024;
uniform int height	= 1024;

in vec2 TexCoord;

out vec2 out_frag0;

void main(void)
{
	vec2 minmax = vec2(1.0e20,-1.0e20);

	const int half_kernel = 10;
	ivec2 tc = ivec2(int(TexCoord.x * width), int(TexCoord.y * height));

	int x=-half_kernel+1;
	int y=-half_kernel+1;

	for(; x<=half_kernel; x++)
	{
		for(; y<=half_kernel; y++)
		{
			float tex = texelFetch(texture0, tc + ivec2(x,y), lod).r;
			minmax.x = min(minmax.x, tex);
			minmax.y = max(minmax.y, tex);
		}
	}

	out_frag0 = minmax;
}