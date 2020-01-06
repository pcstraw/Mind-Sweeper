#version 440
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_uv;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 viewmodel;

out vData
{
vec4 pos;
vec4 normal;
vec3 uv;
}vertex;

void main(void)
{
	vertex.pos = vec4(in_position,1.0); //world space pos
	vertex.normal =  vec4(in_normal,0.0);
	vertex.uv = vec3(in_uv,0.0);
}