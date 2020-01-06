#version 440

layout (location = 0) in vec3 position;

out struct vData
{
	vec3 position;
}vertex;

uniform vec4 colour;
uniform mat4 projection;
uniform mat4 model;

void main()
{
	vertex.position = position;
}