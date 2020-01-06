#version 440

in struct fData
{
	vec4 colour;
	vec4 pos;
}frag;

in flat uint axisid;

layout (location = 0) out vec4 outcolour;
layout (location = 1) out vec4 outposition;
layout (location = 2) out uvec4 outid;

uniform int id;

void main()
{
	vec4 col = frag.colour;
	outcolour = col;
	outposition = frag.pos;
	outid = uvec4(id,axisid,0,0);
}