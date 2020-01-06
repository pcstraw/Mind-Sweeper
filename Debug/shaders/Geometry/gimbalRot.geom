#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
layout(invocations = 3) in;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 viewmodel;
uniform mat4 moffset[3];
uniform vec4 coffset[3];

uniform vec4 selectionColour;
uniform int hitID;

out flat uint axisid;

in vData
{
	vec4 pos;
	vec4 normal;
	vec3 uv;
}vertices[];

out fData
{
	vec4 pos;
	vec4 normal;
	vec3 uv;
	vec4 colour;
	vec3 cameraRelPos;
	flat int pointIndex;
}frag;


void EmitPoint(int index)
{
	mat4 lworld = model*moffset[gl_InvocationID];

	frag.pos = lworld*vertices[index].pos;
	frag.normal = transpose(inverse(lworld))*vertices[index].normal;
	frag.uv = vertices[index].uv;

	vec4 col = coffset[gl_InvocationID];
	int partID = gl_InvocationID + 1;
	if(partID == hitID)
		col = selectionColour;
	frag.colour = col;
	gl_Position = projection*frag.pos;
	axisid = partID;
	EmitVertex();
}

void main() 
{
	for(int i=0;i<3;i++)
	{
		EmitPoint(i);
	}
	EndPrimitive();
}