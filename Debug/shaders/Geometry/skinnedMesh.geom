#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vData
{
	vec4 pos;
	vec4 normal;
	vec3 uv;
	vec4 weights;
	vec3 vertexNormal;
	vec3 cameraRelPos;
	int instance;
}vertices[];

out fData
{
	vec4 pos;
	vec4 normal;
	vec3 uv;
	vec4 weights;
	vec3 vertexNormal;
	vec3 cameraRelPos;
}frag;


void main() 
{
	for(int i = 0; i < 3; i++)
	{
		vec4 pos = gl_in[i].gl_Position;
		frag.normal = vertices[i].normal;
		frag.uv = vertices[i].uv;
		frag.pos = vertices[i].pos;
		frag.cameraRelPos = vertices[i].cameraRelPos;
		frag.vertexNormal = vertices[i].vertexNormal;	
		frag.weights = vertices[i].weights;
		gl_Position = pos;
		EmitVertex();
	}
	EndPrimitive();
}