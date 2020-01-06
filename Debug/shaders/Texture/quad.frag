#version 440

in vec2 uv;
out vec4 outcolour;
uniform sampler2D tex;
uniform vec2 screensize;
uniform vec4 colour;
void main()
{
	vec4 indiff = texture(tex,uv);
	if(indiff.a <= 0.01)
	discard;

	outcolour = indiff * colour;
}