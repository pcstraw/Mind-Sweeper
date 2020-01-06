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

uniform vec4 colour;
uniform int colourid;
out vec4 out_colour;
uniform bool showNormals;
uniform bool showWinding;

void main(void) 
{
	vec4 col = vec4(vertex.normal.xyz * 0.5 + 0.5,1);
	if(!gl_FrontFacing)
		col = vec4(0.0,0.0,0.0,1.0);
	//if(gl_FrontFacing)
	//	discard;
	//if(showWinding)
	//col += vertex.colour;
	out_colour = vec4(col.xyz,1);
}