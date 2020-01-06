#version 440


in struct fData
{
	vec4 colour;
}frag;

out vec4 outcolour;

void main()
{
	vec4 col = frag.colour;
	if(!gl_FrontFacing)
		col = vec4(1.0);
	outcolour = col;
}