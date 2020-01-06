#version 440
		
layout (location = 0) in vec2 Position; 
uniform mat4 projection;
uniform mat4 model;
const vec2 quadVertices[4] = { vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(0.0, 1.0), vec2(1.0, 1.0) };
const vec2 quadUVs[4] = { vec2(1.0, 1.0), vec2(0.0, 1.0), vec2(1.0, 0.0), vec2(0.0, 0.0) };
out vec2 uv;
uniform vec2 size;

void main()
{
	uv = quadUVs[gl_VertexID];
	vec4 coord = projection*model*vec4(quadVertices[gl_VertexID]*size, 0.0, 1.0);
	gl_Position = coord;
} 