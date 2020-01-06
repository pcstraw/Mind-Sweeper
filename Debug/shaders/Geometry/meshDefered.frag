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
layout (location = 0) out vec4 WorldPosOut; 
layout (location = 1) out vec4 NormalOut; 
layout (location = 2) out vec4 DiffuseOut; 
layout (location = 3) out vec4 TexCoordOut;
uniform bool showNormals;
uniform bool showWinding;

void main(void) 
{
	WorldPosOut = vertex.pos;
	//NormalOut = vec4(vertex.normal.xyz,vertex.cameraRelPos.z/farplane);
	NormalOut = vec4(vertex.normal.xyz,length(vertex.cameraRelPos));
	DiffuseOut = colour;
	TexCoordOut = vec4(vertex.uv,1.0);
}