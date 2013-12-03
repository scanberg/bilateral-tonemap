#version 150

uniform sampler2D texture0;

in vec2 TexCoord;

out vec4 out_frag0;

void main(void)
{
	vec4 textureColor = texture(texture0, TexCoord);
	out_frag0 = textureColor;
}