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
vec4 clippos;
vec4 normal;
vec3 uv;
vec3 cameraRelPos;
}vertex;

void main(void)
{
	vec3 inpos = in_position;
	mat4 trans = model;
	vec4 pos = projection * model*vec4(inpos,1.0); //final projected clip pos
	gl_Position = pos;
	vertex.clippos = pos;
	vertex.pos = trans * vec4(inpos,1.0); //world space pos
	vec4 normal = vec4(in_normal,0.0);
	vertex.normal =  transpose(inverse(trans))*normal;
	vertex.uv = vec3(in_uv,0.0);
	vertex.cameraRelPos = (viewmodel*vec4(inpos,1.0)).xyz; //camera relative pos
}