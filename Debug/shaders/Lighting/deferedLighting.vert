#version 440
		
layout (location = 0) in vec2 Position; 
uniform mat4 inverseprojection;
uniform float farplane;
uniform mat4 view;
const vec2 quadVertices[4] = { vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0) };
out vec3 viewDirection;
out flat int instanceID;
in int gl_InstanceID;

void main()
{
	vec4 coord = vec4(quadVertices[gl_VertexID], 0.0, 1.0);
	gl_Position = coord;
	viewDirection = (inverseprojection*coord*view).xyz;
	instanceID = gl_InstanceID;
	//viewDirection = (inverseprojection*coord).xyz;
} 