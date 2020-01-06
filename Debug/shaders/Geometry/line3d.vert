#version 440

const vec3 vertices[2] = { vec3(0.0, 0.0,0.0), vec3(0.0,0.0,1.0)  };

out struct vData
{
	vec3 position;
}vertex;

void main()
{
	vertex.position = vertices[gl_VertexID]; 
}