#version 440
layout(points) in;
layout(triangle_strip, max_vertices = 64) out;
layout(invocations = 4) in;

in struct vData
{
	vec3 position;
}vertex[];

out struct fData
{
	vec4 colour;
}frag;

uniform mat4 projection;
uniform mat4 model;
float scale;

void EmitPoint(vec3 point,vec4 colour)
{
	gl_Position = projection*model*vec4(point,1.0);
	frag.colour = colour;
	EmitVertex();
}

void EmitTriangleWinding(vec3 a, vec3 b ,vec3 c,vec4 colour)
{
	EmitPoint(a,colour + vec4(-1.0,-1.0,-1.0,0.0));
	EmitPoint(b,colour);
	EmitPoint(c,colour + vec4(1.0));
	EndPrimitive();
}

void EmitTriangle(vec3 a, vec3 b ,vec3 c,vec4 colour)
{
	EmitPoint(a,colour);
	EmitPoint(b,colour);
	EmitPoint(c,colour);
	EndPrimitive();
}

void EmitCube(vec3 offset, vec4 colour)
{
	float cubeScale = scale;
	float y = cubeScale;
	vec3 centre = vec3(scale * 0.5);
	vec3 at = centre + vec3(-cubeScale,y,-cubeScale);
	vec3 bt = centre + vec3(-cubeScale,y,cubeScale);
	vec3 ct = centre + vec3(cubeScale,y,cubeScale);
	vec3 dt = centre + vec3(cubeScale,y,-cubeScale);
	y = -cubeScale;
	vec3 ab = centre + vec3(-cubeScale,y,-cubeScale);
	vec3 bb = centre + vec3(-cubeScale,y,cubeScale);
	vec3 cb = centre + vec3(cubeScale,y,cubeScale);
	vec3 db = centre + vec3(cubeScale,y,-cubeScale);

	//top
	EmitTriangle(at,bt,ct,colour);
	EmitTriangle(ct,dt,at,colour);

	//bottom
	EmitTriangle(cb,bb,ab,colour);
	EmitTriangle(ab,db,cb,colour);

	//left x
	EmitTriangle(at,ab,bb,colour);
	EmitTriangle(bb,bt,at,colour);

	//front z
	EmitTriangle(bt,bb,cb,colour);
	EmitTriangle(cb,ct,bt,colour);

	//right x
	EmitTriangle(ct,cb,db,colour);
	EmitTriangle(db,dt,ct,colour);

	//back z
	EmitTriangle(dt,db,ab,colour);
	EmitTriangle(ab,at,dt,colour);
}

void EmitCuboid(vec3 offset,vec3 axisScale, vec4 colour)
{
	float y = 1.0;
	vec3 origin = vec3(0.0);
	vec3 at = (origin + (vec3(0.0,y,0.0) * axisScale))*scale;
	vec3 bt = (origin + (vec3(0.0,y,1.0) * axisScale))*scale;
	vec3 ct = (origin + (vec3(1.0,y,1.0) * axisScale))*scale;
	vec3 dt = (origin + (vec3(1.0,y,0.0) * axisScale))*scale;
	y = 0.0;
	vec3 ab = (origin + (vec3(0.0,y,0.0) * axisScale))*scale;
	vec3 bb = (origin + (vec3(0.0,y,1.0) * axisScale))*scale;
	vec3 cb = (origin + (vec3(1.0,y,1.0) * axisScale))*scale;
	vec3 db = (origin + (vec3(1.0,y,0.0) * axisScale))*scale;

	//top
	EmitTriangle(at,bt,ct,colour);
	EmitTriangle(ct,dt,at,colour);

	//bottom
	EmitTriangle(cb,bb,ab,colour);
	EmitTriangle(ab,db,cb,colour);

	//left x
	EmitTriangle(at,ab,bb,colour);
	EmitTriangle(bb,bt,at,colour);

	//front z
	EmitTriangle(bt,bb,cb,colour);
	EmitTriangle(cb,ct,bt,colour);

	//right x
	EmitTriangle(ct,cb,db,colour);
	EmitTriangle(db,dt,ct,colour);

	//back z
	EmitTriangle(dt,db,ab,colour);
	EmitTriangle(ab,at,dt,colour);
}


void main()
{
	scale = 1.0;
	float clength = 10.0*scale;
	int invID = gl_InvocationID;
	if(invID == 0)
		EmitCuboid(vertex[0].position, vec3(1.0,1.0,clength),vec4(0,0,1,1));
	else if(invID == 1)
		EmitCuboid(vertex[0].position, vec3(clength,1.0,1.0),vec4(1,0,0,1));
	else if(invID == 2)
		EmitCuboid(vertex[0].position, vec3(1.0,clength,1.0),vec4(0,1,0,1));
	else if(invID == 3)
		EmitCube(vertex[0].position,vec4(1.0));
}