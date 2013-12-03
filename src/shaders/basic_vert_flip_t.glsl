#version 150

in vec3 in_position;

out vec2 TexCoord;
 
void main(void)
{
	gl_Position = vec4(in_position, 1.0);
	TexCoord = in_position.xy * vec2(0.5, -0.5) + vec2(0.5, 0.5);
}