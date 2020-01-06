#version 440
			
layout(binding=5) uniform  sampler3D sdftex;
uniform vec2 screensize;
uniform vec3 volumeSize;
uniform vec3 eyepos;
uniform float farplane;
const float EPSILON = 1e-30;
uniform float samplingFactor = 0.5f;

in vec3 viewDirection;
in flat int instanceID;
uniform mat4 projection;
uniform mat4 view;
uniform vec3 camright;
uniform vec3 camup;
uniform mat4 perspective;
uniform mat4 debuglight;
vec3 rayOrigin;
const float M_PI = 3.14159265359;

layout (location = 0) out vec4 outColour;

bool WithinVolume(vec3 pos,float volume)
{
	if(pos.x > 0 && pos.x < volume && pos.y > 0 && pos.y < volume && pos.z > 0 && pos.z < volume)
	return true;
	return false;
}

vec3 opRep( in vec3 p, in vec3 c )
{
    vec3 q = mod(p+0.5*c,c)-0.5*c;
	return q;
}

vec3 opCheapBend(in vec3 p )
{
    const float k = 0.02; // or some other amount
    float c = cos(k*p.x);
    float s = sin(k*p.x);
    mat2  m = mat2(c,-s,s,c);
    vec3  q = vec3(m*p.xy,p.z);
	return q;
}

vec3 opFold(vec3 z,vec3 n1)
{
	z -= 2.0 * min(0.0, dot(z, n1)) * n1;
	return z;
}

vec3 opRepLim( in vec3 p, in vec3 c, in vec3 l )
{
    return p-c*clamp(round(p/c),-l,l);
}

vec3 opRepLim( in vec3 p, in float c, in vec3 l )
{
    return p-c*clamp(round(p/c),-l,l);
}

float opUnion( float d1, float d2 ) { return min(d1,d2); }

float opSubtraction( float d1, float d2 ) { return max(-d1,d2); }

float opIntersection( float d1, float d2 ) { return max(d1,d2); }

float sdSphere(vec3 p,vec3 centre,float radius)
{
	return distance(p,centre) - radius;
}
float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return length(max(d,0.0))
         + min(max(d.x,max(d.y,d.z)),0.0); // remove this line for an only partially signed sdf 
}
float sdBox( vec3 p, vec3 pos,vec3 b )
{
	vec3 d = abs(p - pos) -  b;
	return  length(max(d,0.0))
			+ min(max(d.x,max( d.y,d.z)),0.0);
}
float sdPlane( vec3 p, vec4 n )
{
	// n must be normalized
	return dot(p,n.xyz) + n.w;
}

float sceneSDF(vec3 pos)
{
	float f = farplane;
	vec3 p = pos;
	p = opRep(pos,vec3(0.0,0.0,15.0));
	//p = opCheapBend(p);
	//p = opRepLim(pos,2.0,vec3(10,10,10));
	//f = opUnion(sdSphere(p,vec3(0),10.0),f);
	f = opUnion(sdBox(p,vec3(0,-5,0),vec3(15,1,15)),f);
	
	float sideoffset = 40.0;
	float zdist = 50.0;
	vec3 p1 = vec3(sideoffset,0.0,0.0) + opRep(pos,vec3(0.0,0.0,zdist));
	f = opUnion(sdSphere(p1,vec3(0),10.0),f);
	f = opUnion(sdBox(p1,vec3(0,-5,0),vec3(15,1,15)),f);

	//vec3 p2 = opFold(p1,vec3(0.0,1.0,0.0));
	vec3 p2 = vec3(-sideoffset,0.0,0.0) + opRep(pos,vec3(0.0,0.0,zdist));
	f = opUnion(sdSphere(p2,vec3(0),10.0),f);
	f = opUnion(sdBox(p2,vec3(0,-5,0),vec3(15,1,15)),f);

	return f;
}

/*
float sceneSDF(vec3 pos)
{
	float f = farplane;
	vec3 p = pos;
	p = opRep(pos,vec3(50.0,0.0,50.0));
	//p = opRepLim(pos,2.0,vec3(10,10,10));
	f = opUnion(sdSphere(p,vec3(0),10.0),f);
	f = opUnion(sdBox(p,vec3(0,-5,0),vec3(15,1,15)),f);
	//f = opUnion(sdPlane(p,vec4(0,1,0,0)),f);
	return f;
	//return sampleSDF(p).a;
}
*/

vec3 getNormal (vec3 p)
{
	const float eps = 0.5;
	return normalize
	( vec3
	( sceneSDF(p + vec3(eps, 0, 0) ) - sceneSDF(p - vec3(eps, 0, 0)),
	sceneSDF(p + vec3(0, eps, 0) ) - sceneSDF(p - vec3(0, eps, 0)),
	sceneSDF(p + vec3(0, 0, eps) ) - sceneSDF(p - vec3(0, 0, eps))
	)
	);
}

float shadowSDF( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
{
    float res = 1.0;
    float ph = 1e20;
	//float threshold = 10e-6;
	float threshold = 0.001;
    for( float t=mint; t<maxt; )
    {
        float h = sceneSDF(ro + rd*t);
        if( h < threshold )
            return 0.0;
        float y = h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, k*d/max(0.0,t-y) );
        ph = h;
        t += h;
    }
    return res;
}

float LinearizeDepth(float z)
{
  float n = 1.0; // camera z near
  float f = farplane; // camera z far
  return (2.0 * n) / (f + n - z * (f - n));	
}

bool RayMarch(vec3 ro, vec3 rd, float far,out vec3 position)
{
	float dist = 0;
	vec3 p = ro;
	//float threshold = 10e-6;
	float threshold = 0.00001;
	for(int i=0;i<512;i++)
	{
		vec3 pos = p + rd * dist;
		float nearest =  sceneSDF(pos);
		position = pos;
		if(nearest < threshold*dist)
			return true;
		dist += nearest;
	}

	return false;
}

void main()
{         
	//todo normalize these on the cpu
	vec3 eye = eyepos;
	vec3 up = camup;
	vec3 right = normalize(camright);
	vec3 forward = normalize(cross(right, up));
	float aspectRatio = screensize.x / screensize.y;
	float u = (((gl_FragCoord.x + 0.5)*aspectRatio) * 2.0) / screensize.x - 1.0;
	float v = ((gl_FragCoord.y + 0.5) * 2.0) / screensize.y - 1.0;

	//ray direction.  Note -forward : not certain yet why this works
	vec3 ro = eye + forward*10.0 + (right * u + up * v);
	rayOrigin = ro;

	vec3 rd = normalize(viewDirection);
	vec3 colour = rd;
	vec3 pos = vec3(0);
	outColour = vec4(rd,1);
	
	if(RayMarch(ro,rd,10000,pos))
	{
		vec4 clip_pos = perspective * vec4(pos,1);
		float clip_depth = clip_pos.z/clip_pos.w * 0.5 + 0.5;
		gl_FragDepth = clip_depth;


		vec3 normal = getNormal(pos);
		vec3 lightpos = debuglight[0].xyz;
		vec4 lightcol = debuglight[1];
		vec3 lightdir = normalize( lightpos - pos );
		vec3 halfl = normalize(lightdir - rd);
		float specular = debuglight[0].w;

		float diff = clamp( dot( normal, lightdir ), 0.0, 1.0 );

		float spec = pow( clamp( dot( normal, halfl ), 0.0, 1.0 ),16.0)* diff *
                    (0.04 + 0.96*pow( clamp(1.0 + dot(halfl,rd),0.0,1.0), specular ));

		float shadow = shadowSDF(pos,lightdir,10.1,10000.0,0.8);
		//float shadow = 1.0;
		vec4 col = vec4(((normal + 0.5 * 0.5) + vec3(spec) )*shadow ,1.0);// + (lightcol*spec);
		outColour = col * lightcol;
		return;
	}
	outColour = vec4(rd,1);
}