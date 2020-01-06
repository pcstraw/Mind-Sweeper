#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vData
{
	vec4 pos;
	vec4 clippos;
	vec4 normal;
	vec3 uv;
	vec3 cameraRelPos;
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
	gl_Position = vertices[index].clippos;
	frag.pos = vertices[index].pos;
	frag.normal = vertices[index].normal;
	frag.uv = vertices[index].uv;
	frag.cameraRelPos = vertices[index].cameraRelPos;
	frag.pointIndex = index;
	frag.colour = vec4(index) - vec4(0.5);
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