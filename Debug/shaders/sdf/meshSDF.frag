#version 440
			
			//layout (location = 0) in vec2 Position; 
	
			layout(binding=1) uniform sampler2DRect postex;
			layout(binding=2) uniform sampler2DRect normtex;
			layout(binding=3) uniform sampler2DRect difftex;
			layout(binding=4) uniform sampler2DRect uvtex;
			layout(binding=5) uniform sampler3D volumetex;
			layout(binding=4) uniform  sampler3D sdftex;
			layout(binding=6) uniform isampler3D indextex;
			layout(binding=7, rgba16f) uniform readonly image3D voltex;
			layout(binding=9) uniform sampler2D depthTex;
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
			//vec3 vd;
			vec3 rayOrigin;
			const float M_PI = 3.14159265359;

			//layout (location = 1) out vec4 outShadow;
			layout (location = 0) out vec4 outColour;

			const vec3[7] offsets = { vec3(0), vec3(1,0,0), vec3(0,1,0), vec3(0,0,1),vec3(-1,0,0),vec3(0,-1,0),vec3(0,0,-1) };

		bool WithinVolume(vec3 pos,float volume)
		{
			if(pos.x > 0 && pos.x < volume && pos.y > 0 && pos.y < volume && pos.z > 0 && pos.z < volume)
			return true;
			return false;
		}

			bool IntersectBounds(vec3 ro, vec3 rd,vec3 boxmin, vec3 boxmax, out float tnear,out float tfar)
	{
		// compute intersection of ray with all six bbox planes
		vec3 invR = 1.0 / rd;
		vec3 tbot = invR * (boxmin - ro);
		vec3 ttop = invR * (boxmax - ro);

		// re-order intersections to find smallest and largest on each axis
		vec3 tmin = min (ttop, tbot);
		vec3 tmax = max (ttop, tbot);

		// find the largest tmin and the smallest tmax
		vec2 t0 = max (tmin.xx, tmin.yz);
		tnear = max (t0.x, t0.y);
		t0 = min (tmax.xx, tmax.yz);
		tfar = min (t0.x, t0.y);

		// check for hit
		bool hit;
		if (tnear > tfar)
			hit = false;
		else
			hit = true;
		return hit;
	}

	vec4 GetVoxel(vec3 pos)
	{
		vec3 pixel =  (pos*0.5) + vec3(0.5);
		//return texture(volumetex, pixel/volumeSize);
		return texelFetch(volumetex, ivec3(pos), 0);
	}

	ivec4 GetVoxelID(vec3 pos)
	{
	//	vec3 pixel =  (pos*0.5f)+vec3(0.5);
	//	return ivec4(texture(indextex, pixel/256));
		return texelFetch(indextex,ivec3(pos),0);
	}

	vec4 GetVoxelSDF(vec3 pos)
	{
		//vec3 pixel =  (pos) - vec3(0.5);
		vec3 pixel = pos - vec3(0.5);
		vec4 v = texture(sdftex, (pixel/volumeSize));
		return v;
		//return texelFetch(sdftex, ivec3(pos),0);
	}

	bool IntersectBox(vec3 ro, vec3 rd,vec3 boxmin, vec3 boxmax, out float tnear,
			out float tfar)
			{
				// compute intersection of ray with all six bbox planes
				vec3 invR = 1.0 / rd;
				vec3 tbot = invR * (boxmin - ro);
				vec3 ttop = invR * (boxmax - ro);

				// re-order intersections to find smallest and largest on each axis
				vec3 tmin = min (ttop, tbot);
				vec3 tmax = max (ttop, tbot);

				// find the largest tmin and the smallest tmax
				vec2 t0 = max (tmin.xx, tmin.yz);
				tnear = max (t0.x, t0.y);
				t0 = min (tmax.xx, tmax.yz);
				tfar = min (t0.x, t0.y);

				// check for hit
				bool hit;
				if (tnear > tfar)
					hit = false;
				else
					hit = true;
				return hit;
			}

	
	float sceneSDF(vec3 pos)
	{
		float f= GetVoxelSDF(pos).a;
		//float f = RaymarchSDFTex(pos);
		return f;
	}

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

	vec3 WorldToVoxel(vec3 position)
	{
		float voxelScale = 1.0f;
		vec3 worldMinPoint = vec3(1);
		vec3 voxelPos = position - worldMinPoint;
		return voxelPos * voxelScale;
	}

	bool IntersectRayWithWorldAABB(vec3 ro, vec3 rd, out float enter, out float leave)
	{
		vec3 worldMinPoint = vec3(1);
		vec3 worldMaxPoint = vec3(256);

		vec3 tempMin = (worldMinPoint - ro) / rd; 
		vec3 tempMax = (worldMaxPoint - ro) / rd;
    
		vec3 v3Max = max (tempMax, tempMin);
		vec3 v3Min = min (tempMax, tempMin);
    
		leave = min (v3Max.x, min (v3Max.y, v3Max.z));
		enter = max (max (v3Min.x, 0.0), max (v3Min.y, v3Min.z));    
    
		return leave > enter;
	}

	vec3 GetAmbient(vec3 lightcolour,vec3 objectcolor,vec3 Normal,vec3 lightPos,vec3 FragPos)
	{
		float strength = 0.1;
		vec3 ambient = lightcolour*strength;

		vec3 norm = normalize(Normal);
		vec3 lightDir = normalize(lightPos - FragPos);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = diff * lightcolour;

		float specularStrength = 1.0;
		vec3 viewDir = normalize(eyepos - FragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), 50);
		vec3 specular = specularStrength * spec * lightcolour;  

		return (ambient+diffuse+specular)*objectcolor;
	}

	float sphereDistance(vec3 p,vec3 centre,float radius)
	{
		return distance(p,centre) - radius;
	}

	float TraceShadow2(vec3 position, vec3 direction, float maxTracingDistance,out float distanceTraveled,out vec4 outVoxel) 
	{

		//return vec4(position,1);
		float voxelTexSize = 100.0f / 256;
		float dist_place = (voxelTexSize*0.5f)+0.5f;
		// move one voxel further to avoid self collision
		
		//float step = (voxelTexSize);
		float step = (maxTracingDistance/256);
		float dist = 0;
		//dist = 0.0;
		//float hit = -1.0f;

		float traceSample = 0.0f;
		const float EPSILON = 1e-30;

		vec3 sampleP;
		vec4 voxel;
		int startID = (GetVoxelID(position).r); 
		
		if(startID  < 0)
			return 1.0;
			
		float intensity = 0.02;
		int iteration = 1;
		float visibility = 1;
		bool contact = false;
		float res = 1.0;
		float k = 0.5;
		float t = 0.0;
		while(res >= 0.001 && dist < maxTracingDistance)
		{	
			sampleP = (position + offsets[instanceID]*iteration*0.1) + direction * dist;
			if (!WithinVolume(sampleP,256))
				return res;

			voxel = GetVoxel(sampleP);
			if(voxel.a> 0.9)
			{
				int nextID = GetVoxelID(sampleP).r;
				if(nextID >= 0 && nextID != startID)
				{
					res -= 0.1;
				}
			}
			t += step;
			dist += step;
			iteration ++;
		}
		distanceTraveled = dist;//iteration;
		outVoxel = voxel;
		//if(res ==  0.0)
		//	return 1.0f;
		if(res < 1)
			res = max(0,(res/maxTracingDistance));
		return res;
	}

	float opUnion(float dist1, float dist2)
	{
		return min(dist1,dist2);
	}

	float sdBall(vec3 p,vec3 centre,float radius)
	{
		  return length(p - centre) - radius;
	}

	float sdBoxPos( vec3 p, vec3 pos,vec3 b )
	{
	  vec3 d = abs(p - pos) -  b;
	  return  length(max(d,0.0))
			 + min(max(d.x,max( d.y,d.z)),0.0);
	}

	float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return length(max(d,0.0))
         + min(max(d.x,max(d.y,d.z)),0.0); // remove this line for an only partially signed sdf 
}

	vec4 RayMarchPS(vec3 point, vec3 ro,vec3 rd,out vec3 pos,out vec3 Pnear,out vec3 Pfar)
	{

		float tnear=0;
		float tfar=0;
		bool hit = IntersectBox(ro,rd, vec3(1), vec3(255), tnear, tfar);
				
		if (!hit) 
		{
			
			return vec4(farplane);
		}
		
		// calculate intersection points
		Pnear = ro + rd*tnear;
		Pfar = ro + rd*tfar;
		Pnear = (Pnear * 0.5) + 0.5;
		Pfar = (Pfar * 0.5) + 0.5;
			
		vec4 c = vec4(0.0);
		int steps = 64;
		vec3 step = (Pnear - Pfar) / (steps-1);
		vec3 dir = normalize(Pnear - Pfar);
		//vec3 P = Pfar;
		vec3 P = Pnear;
		float dist = farplane;
		float t = 0;
		int variableStep = int(length(Pnear - Pfar));

		for(int i=0; i < steps; i++)
		{
			if(!WithinVolume(P,255))
				break;
			vec4 s3d = GetVoxelSDF(P);
			if (s3d.a <= 1.0)
			{
				vec3 substep = P;
				for(int j = 0;j < 16;j++)
				{
					vec4 voxel = GetVoxelSDF(substep);
					if(voxel.a < 0.5)
						return voxel;
					substep -= dir*0.5;
				}
			}

			P -= step;
					
			if(s3d.a >= farplane)
				break;
		}
		return vec4(farplane);
	}

	

	float hardshadow(vec3 ro,vec3 lightPos)
	{
		float t=1.0;
		vec3 rd = normalize(lightPos - ro); //light direction
		float light_distance = length(lightPos - ro);
		for(int i = 0; i < 32 ; i++)
		{
			float f = sceneSDF(ro + rd * t);
			if( f <= 0)
			return 0;

			if(f >= light_distance)
			return 1;

			t += f;
		}

		return 0;
	}

	float softshadow2(vec3 ro,vec3 lightPos)
	{
		float t = 0.01;
		vec3 rd = normalize(lightPos - ro); //light direction
		float light_distance = length(lightPos - ro);
		float nearest = farplane;
		float hardness = 0.5;
		for(int i = 0; i < 64 ; i++)
		{
			float f = sceneSDF(ro + rd * t);
			if( f < 0.01)
				return 0.01;

			if(f >= light_distance)
				return clamp(nearest,0,1);

			nearest = min(nearest,(f/t)*hardness);

			t += f;
		}

		return 0.0;
	}
	

	float softshadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
{
    float res = 1.0;
    float ph = 1e20;
    for( float t=mint; t < maxt; )
    {
        float h = sceneSDF(ro + rd*t);
        if( h<0.001 )
            return 0.0;
        float y = h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, k*d/max(0.0,t-y) );
        ph = h;
        t += h;
    }
    return res;
}

vec4 RayMarch(vec3 ro, vec3 rd)
{
	vec4 colour = vec4(farplane);
			
	//start ray trace at eye pos (0).  Can probably offset this if we want
	float t = 0.0;

	//smaller step sizes yield warping at the contours of objects.  For now use high iteration count
	const int maxSteps = 512;

	//minimum step if we choose to use it.  Can help solve the problem above, yet performance is worse
	float step = 1.0; 

	//our hit condition if naively implemented.  Once this pixel has hit something, we go into precision mode
	//adding seperating the broad from narrow phase into their own loops may help us reduce the total number of steps
	bool hit = false;
	colour = vec4(farplane);	
	for(int i = 0; i < maxSteps; ++i)
	{
		vec3 p = ro + rd * t;
		float d = sceneSDF(p); // Distance to sphere of radius 0.5
		
		//close enough to count as a hit
		if(d <= 1.0)
		{
			//is it worth adding a check first?
			hit = true;
			colour = vec4(p,d);
			//break;
			//surface range
			if(d <= 0.001)
			{
				colour = vec4(p,d);
				break;
			}

			//reduce the distance and send it in the other direction on the next iteration
			//this is why we need a bigger step size:  we don't want to run out of iterations
			//before the narrow phase has had a chance to give us a good another
			float ht = d;
			t += ht;
		}else
		{
			t += d;
		}
		//hit far plane
		if(t >= farplane)
		{
			break;
		}
	}
	return colour;
}



vec4 RaymarchTex(vec3 ro,vec3 rd)
	{
		vec4 colour = vec4(farplane); // Sky color
			
		//start ray trace at eye pos (0).  Can probably offset this if we want
		float t = 0.0;

		//smaller step sizes yield warping at the contours of objects.  For now use high iteration count
		const int maxSteps = 512;

		//minimum step if we choose to use it.  Can help solve the problem above, yet performance is worse
		//float step = 1.0; 

		//our hit condition if naively implemented.  Once this pixel has hit something, we go into precision mode
		//adding seperating the broad from narrow phase into their own loops may help us reduce the total number of steps

		float tnear=0;
		float tfar=0;

		bool hit = IntersectBox(ro,rd, vec3(1), volumeSize - vec3(1.0), tnear, tfar);
				
		if (!hit) 
			return colour;

		// calculate intersection points
		vec3 Pnear = (ro + rd*tnear);
		vec3 Pfar = (ro + rd*tfar);
		//Pnear = (Pnear * 0.5) + 0.5;
		//Pfar = (Pfar * 0.5) + 0.5;


		vec3 Pn = Pnear;
		vec3 Pf = Pfar;
		int steps = 256;
		vec3 step = (Pn - Pf) / (steps-1);
		vec3 P = Pn;
		vec4 start_voxel = GetVoxelSDF(P);
		if(tnear < 0)
			P = ro;

		vec3 dir = normalize(step);
		for(int i=0; i < steps; i++)
		{
			vec4 s3d = GetVoxelSDF(P);
			if (s3d.a <= 1.0)
			{
				vec3 substep = P + dir;
				for(int j = 0; j < 128; j++)
				{

					vec4 voxel = GetVoxelSDF(substep);
					if(voxel.a <= 0.999)
						return vec4(substep,voxel.a);
					substep -= step/32; //smaller step size for small high detailed meshes
					//if(!WithinVolume(substep,255))
					//	break;
				}
			}

			P -= step;
			if(!WithinVolume(P,255))
				break;
			if(s3d.a >= farplane)
				break;
		}
		return colour;
	}
		float LinearizeDepth(float z)
{
  float n = 1.0; // camera z near
  float f = farplane; // camera z far
  return (2.0 * n) / (f + n - z * (f - n));	
}

		void main()
		{             
			vec4 diff = texture2DRect(difftex,gl_FragCoord.xy);
			vec3 worldray = eyepos + normalize(viewDirection)*instanceID;
			vec4 pos = texture2DRect(postex,gl_FragCoord.xy);
			float _depth = LinearizeDepth(pos.w);
			//float _depth = pos.w;
			vec4 normal = texture2DRect(normtex,gl_FragCoord.xy);
			vec3 lightpos = vec3(200.0,200.0,200.0);
			vec3 lightcolour = vec3(1.0);

			//outColour = vec4(worldray,1.0);
			if(diff.a > 0.0)
			{
			
				//vec3 viewRay = vec3(viewDirection.xy * (farplane/viewDirection.z),farplane);
				//vec3 depth_pos = eyepos+viewRay*normal.a;
				vec3 depth_pos = eyepos + (normalize(viewDirection)*normal.a);
				//vec3 depth_pos = pos.xyz;
				vec4 uv = texture2DRect(uvtex,gl_FragCoord.xy);
			
				if(WithinVolume(pos.xyz,256.0))
				{
					vec4 voxel = GetVoxel(pos.xyz);
					if(voxel.a > 0)
					{
					//	outColour = vec4(GetAmbient(lightcolour,diff.xyz,normal.xyz,lightpos,pos.xyz),1.0);
					}
				}
			//	outColour = vec4(GetAmbient(lightcolour,diff.xyz,normal.xyz,lightpos,pos.xyz),1.0);
			//	outColour = diff;
			}

			//todo normalize these on the cpu
			vec3 eye = eyepos;
			vec3 up = camup;
			vec3 right = normalize(camright);
			vec3 forward = normalize(cross(right, up));
			float aspectRatio = screensize.x / screensize.y;

			//float u = (2.0 * ((gl_FragCoord.x + 0.5) / 1080) - 1.0) * tan(radians(90.0) / 2.0 * M_PI / 180) * aspectRatio; 
			//float v = (1.0 - 2.0 * (gl_FragCoord.y + 0.5 / 720)) * tan(radians(90.0) / 2.0 * M_PI / 180); 

			//get screencoords in camera space (UVs)
			//2.0 is the length of NDC range -1 to 1
			//+0.5 to reach pixel centre
			float u = (((gl_FragCoord.x + 0.5)*aspectRatio) * 2.0) / screensize.x - 1.0;
			float v = ((gl_FragCoord.y + 0.5) * 2.0) / screensize.y - 1.0;

			//ray direction.  Note -forward : not certain yet why this works
			vec3 ro = eye - forward*2.0 + (right * u + up * v);
			rayOrigin = ro;

			vec3 rd = normalize(viewDirection);
			
			vec4 rayMarchedColour = RaymarchTex(rayOrigin,rd);

			if(rayMarchedColour.a < 1.0)
			{
				vec3 shadow_dir = normalize(lightpos - rayMarchedColour.xyz);
				float s = softshadow( rayMarchedColour.xyz,shadow_dir , 0.01, 10, 1.5 );
				float ss = softshadow2(rayMarchedColour.xyz,lightpos);

				vec4 clip_pos = perspective * vec4(rayMarchedColour.xyz,1);
				float clip_depth = clip_pos.z/clip_pos.w * 0.5 + 0.5;
				outColour = vec4((getNormal(rayMarchedColour.xyz)* 0.5 + 0.5),1);
				gl_FragDepth = clip_depth;
			}
			
			
			//vec4 rm = RayMarch(rayOrigin,rd);
			//if(rm.a < farplane)
			//{
			//	vec3 shadow_dir = normalize(lightpos - rm.xyz);
				//float s = softshadow( rm.xyz,shadow_dir , 0.01, 100.0, 0.5 );
				//float hs = hardshadow(rm.xyz,lightpos);
			//	float ss = softshadow2(rm.xyz,lightpos);
			//	outColour = vec4(getNormal(rm.xyz)* 0.5 + 0.5,1.0)*ss;
			//}
			
		//	outColour = vec4(vec3(_depth*77.0),1.0);
		}