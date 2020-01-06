#version 440
layout(triangles) in;
layout(triangle_strip, max_vertices = 64) out;
layout(invocations = 7) in;

in struct vData
{
	vec3 position;
}vertex[];

out struct fData
{
	vec4 colour;
	vec4 pos;
}frag;

out flat uint axisid;
uint _aid;

uniform mat4 projection;
uniform mat4 model;
uniform float scale;
uniform float axisLength;
uniform int hitID;
uniform vec4 selectionColour;

void EmitPoint(vec3 point,vec4 colour)
{
	vec4 wpos = model*vec4(point,1.0);
	gl_Position = projection*wpos;
	frag.pos = wpos;
	frag.colour = colour;
	axisid = _aid;
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

void EmitCube(vec4 colour)
{
	float halfScale = scale*0.5;
	float s = scale + halfScale;
	//vec3 scaleOffset = vec3(scale*0.25);
	vec3 scaleOffset = vec3(halfScale*0.5);
	for(int i=0;i<3;i++)
	{
		vec3 cs = vertex[i].position * s;
		vec3 pos = cs - scaleOffset;
		EmitPoint(pos,colour);
	}
}

void EmitCuboid(vec3 axisScale, vec4 colour)
{
	for(int i=0;i<3;i++)
	{
		vec3 pos = vertex[i].position * axisScale;
		EmitPoint(pos,colour);
	}
}

void EmitPlane(vec3 a, vec3 b, vec3 c, vec4 colour)
{
	EmitPoint(a,colour);
	EmitPoint(b,colour);
	EmitPoint(c,colour);
	EndPrimitive();
}


void main()
{
	float clength = axisLength*scale;
	int invID = gl_InvocationID;

	float po = (0.5*scale);
	float planelength = clength;
	float planeoffset = (0.4 * clength);
	if(invID == 0)
	{
		vec4 col = vec4(1.0,0.0,0.0,1.0);
		_aid = 1;
		if(_aid == hitID)
			col = selectionColour;
		EmitCuboid(vec3(clength,scale,scale),col);
	}else if(invID == 1)
	{
		vec4 col = vec4(0.0,1.0,0.0,1.0);
		_aid = 2;
		if(_aid == hitID)
			col = selectionColour;
		EmitCuboid(vec3(scale,clength,scale),col);
	}else if(invID == 2)
	{
		vec4 col = vec4(0.0,0.0,1.0,1.0);
		_aid = 3;
		if(_aid == hitID)
			col = selectionColour;
		EmitCuboid(vec3(scale,scale,clength),col);
	}else if(invID == 3)
	{
		vec4 col = vec4(0.6,0.6,0.6,1.0);
		_aid = 4;
		if(_aid == hitID)
			col = selectionColour;
		EmitCube(col);
	}else if(invID == 4)
	{
		vec3 a = vec3(planeoffset,planeoffset,po);
		vec3 b = vec3(planeoffset,planelength,po);
		vec3 c = vec3(planelength,planeoffset,po);
		vec4 colour = vec4(1,1,0,1);
		_aid = 5;
		if(_aid == hitID)
			colour = selectionColour;
		EmitPlane(a,b,c,colour);
	}else if(invID == 5)
	{
		vec3 a = vec3(po,planeoffset,planeoffset);
		vec3 b = vec3(po,planelength,planeoffset);
		vec3 c = vec3(po,planeoffset,planelength);
		vec4 colour = vec4(0,1,1,1);
		_aid = 6;
		if(_aid == hitID)
			colour = selectionColour;
		EmitPlane(a,b,c,colour);
	}else if(invID == 6)
	{
		vec3 a = vec3(planelength,po,planeoffset);
		vec3 b = vec3(planeoffset,po,planeoffset);
		vec3 c = vec3(planeoffset,po,planelength);
		vec4 colour = vec4(1,0,1,1);
		_aid = 7;
		if(_aid == hitID)
			colour = selectionColour;
		EmitPlane(a,b,c,colour);
	}
}