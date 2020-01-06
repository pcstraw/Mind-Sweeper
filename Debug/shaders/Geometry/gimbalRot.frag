#version 440

in fData
{
	vec4 pos;
	vec4 normal;
	vec3 uv;
	vec4 colour;
	vec3 cameraRelPos;
	flat int pointIndex;
}vertex;

in flat uint axisid;
uniform int id;

layout (location = 0) out vec4 outcolour;
layout (location = 1) out vec4 outposition;
layout (location = 2) out uvec4 outid;

void main(void) 
{
	outcolour = vertex.colour;
	outposition = vertex.pos;
	outid = uvec4(id,axisid,0,0);
}