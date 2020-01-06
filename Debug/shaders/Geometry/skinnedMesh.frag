#version 440

in fData
{
	vec4 pos;
	vec4 normal;
	vec3 uv;
	vec4 weights;
	vec3 vertexNormal;
	vec3 cameraRelPos;
}vertex;

layout (location = 0) out vec4 defaultBufferOut;
layout (location = 1) out vec4 WorldPosOut; 
layout (location = 2) out vec4 NormalOut; 
layout (location = 3) out vec4 DiffuseOut; 
layout (location = 4) out vec4 TexCoordOut;
layout (location = 5) out int Id;

uniform int colourid;

out vec4 _outcolour;

uniform vec4 colour;

void main(void) 
{
	WorldPosOut = vertex.pos;
	//NormalOut = vec4(vertex.normal.xyz,vertex.cameraRelPos.z/farplane);
	NormalOut = vec4(vertex.normal.xyz,length(vertex.cameraRelPos));
	DiffuseOut = colour;
	TexCoordOut = vec4(vertex.uv,1.0);
	Id = colourid;
	vec3 localNormal = vertex.normal.xyz * 0.5 + 0.5;
	_outcolour = vec4(localNormal,1);
}