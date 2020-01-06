#version 440
			
			//layout (location = 0) in vec2 Position; 
	
			layout(binding=1) uniform sampler2DRect postex;
			layout(binding=2) uniform sampler2DRect normtex;
			layout(binding=3) uniform sampler2DRect difftex;
			layout(binding=4) uniform sampler2DRect uvtex;

			layout(binding=5) uniform sampler3D volumetex;
			layout(binding=6) uniform isampler3D indextex;
			layout(binding=7, rgba16f) uniform image3D voltex;

			uniform vec2 screensize;
			uniform vec3 eyepos;
			uniform float farplane;
			const float EPSILON = 1e-30;
			uniform float samplingFactor = 0.5f;

			in vec3 viewDirection;
			in flat int instanceID;
			uniform mat4 projection;
			uniform mat4 view;

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
		//vec3 pixel =  (pos*0.5) - vec3(0.5);
		//return texture(volumetex, pixel/256);
		return texelFetch(volumetex, ivec3(pos), 0);
	}

	ivec4 GetVoxelID(vec3 pos)
	{
	//	vec3 pixel =  (pos*0.5f)+vec3(0.5);
	//	return ivec4(texture(indextex, pixel/256));
		return texelFetch(indextex,ivec3(pos),0);
	}

	vec4 GetVoxelCubic(vec3 pos, int radius)
	{
		vec4 accum = GetVoxel(pos);
		accum += GetVoxel(pos + vec3(radius,0,0));
		accum += GetVoxel(pos + vec3(0,radius,0));
		accum += GetVoxel(pos + vec3(0,0,radius));
		int samples = 0;
		
		return accum/4;
	}

	float SoftShadow( vec3 ro, vec3 rd, float mint, float maxt, float k )
	{
		float res = 0.1;
		float ph = 1e20;
		for( float t = mint; t < maxt; )
		{
			float h = (GetVoxel((ro + rd*t)/256)).a;
			if( h < 0.1 )
				return 0.0;
			float y = h*h/(2.0*ph);
			float d = sqrt(h*h-y*y);
			res = min( res, k*d/max(0.0,t-y) );
			ph = h;
			t += h;
		}
		return res;
	}

	vec4 RayMarchShadow(vec3 ro,vec3 rd,out vec3 pos,out vec3 Pnear,out vec3 Pfar)
	{
		//eyeray.d = normalize(rd); //ray direction should already be normalised
		// calculate ray intersection with bounding box

		//float f = sdBox(point,vec3(0),volumesize);
		//if(f > -2.0)
		//	return vec4(MAX);
		//return vec4(f);

	
		float tnear=0;
		float tfar=0;
		vec3 volumesize = vec3(256);
		bool hit = IntersectBounds(ro,rd, vec3(0), volumesize, tnear, tfar);
				
		if (!hit) 
			return vec4(0);

		//if(f.a >= MAX_DIST)
		//	return vec4(MAX_DIST);

		//if (tnear < 0.0) 
		//	tnear = 0.0;

		// calculate intersection points
		Pnear = ro + rd*tnear;
		Pfar = ro + rd*tfar;
		Pnear = (Pnear * 0.5) + 0.5;
		Pfar = (Pfar * 0.5) + 0.5;
			

		vec4 c = vec4(0.0);
		int steps = 64;
		//vec3 step = (Pnear - Pfar) / (steps-1);
		//vec3 step = normalize(Pnear - Pfar);
		//vec3 dir = normalize(Pnear - Pfar);
		vec3 step = (Pnear - Pfar) / (steps-1);
		//vec3 P = Pfar;
		vec3 P = Pnear;
		//vec3 P = f.xyz;
		//vec3 P = ro;
		//float dist = MAX;
		float t = 0;
		int variableStep = int(length(Pnear - Pfar));
		vec4 voxel;
		for(int i=0; i < steps; i++)
		{
			//P = Pnear + Pfar*t;
			//vec4 next = getVolumeTex(P+step);
			vec4 s3d = texture(volumetex, P/256);
			if (s3d.a > 0.0)
			{
				pos = P;
				voxel = s3d;
				return voxel;
			}

			P -= step;
			//P += step;
			//if(s3d.a >= MAX)
			//	break;
		}
		return voxel;
	}

	float TraceShadow(vec3 position, vec3 direction, float maxTracingDistance) 
	{
		// scaling factor
		float traceShadowHit = 0.1;
		float k = traceShadowHit * traceShadowHit;
		// navigation
		float volumeDimension = 256;
		float voxelTexSize = 1.0f / volumeDimension;
		// move one voxel further to avoid self collision
		float dst = voxelTexSize * 2.0f;
		vec3 samplePos = direction * dst + position;
		// control variables
		float visibility = 0.0f;
		// accumulated sample
		float traceSample = 0.0f;
		const float EPSILON = 1e-30;

		while (visibility <= 1.0f && dst <= maxTracingDistance) 
		{
			if (samplePos.x < 0.0f || samplePos.y < 0.0f || samplePos.z < 0.0f
				|| samplePos.x > 1.0f || samplePos.y > 1.0f || samplePos.z > 1.0f) 
			{ 
				break; 
			}
        
			traceSample = ceil(texture(volumetex, samplePos).a) * k;

			// hard shadows mode
			//if(traceSample > 1.0f - EPSILON) { return 0.0f; }
			if(traceSample >= 1.0f) { return 0.0f; }
			// accumulate
			visibility += (1.0f - visibility) * traceSample / dst;
			// move further into volume
			dst += voxelTexSize;
			samplePos = direction * dst + position;
		}

		return 1.0f - visibility;
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

	float TraceShadowCone(vec3 position, vec3 direction, float aperture, float maxTracingDistance) 
{
    bool hardShadows = false;
	float coneShadowTolerance = 0.4f;
	float voxelScale = 1.0f;
	float volumeDimension = 256.0f;

    if(coneShadowTolerance == 1.0f) { hardShadows = true; }

    // directional dominat axis
    uvec3 visibleFace;
    visibleFace.x = (direction.x < 0.0) ? 0 : 1;
    visibleFace.y = (direction.y < 0.0) ? 2 : 3;
    visibleFace.z = (direction.z < 0.0) ? 4 : 5;
    // world space grid size
    float voxelWorldSize = 2.0 /  (voxelScale * volumeDimension);
    // weight per axis for aniso sampling
    vec3 weight = direction * direction;
    // move further to avoid self collision
    float dst = voxelWorldSize;
    vec3 startPosition = position + direction * dst;
    // control vars
    float mipMaxLevel = log2(volumeDimension) - 1.0f;
    // final results
    float visibility = 0.0f;
    float k = exp2(7.0f * coneShadowTolerance);
    // cone will only trace the needed distance
    float maxDistance = maxTracingDistance;
    // out of boundaries check
    float enter = 0.0; float leave = 0.0;
	vec4 sampleIndex = GetVoxelID(position);
    if(!IntersectRayWithWorldAABB(position, direction, enter, leave))
    {
        visibility = 1.0f;
    }
    
    while(visibility < 1.0f && dst <= maxDistance)
    {
        vec3 conePosition = startPosition + direction * dst;
        float diameter = 2.0f * aperture * dst;
        float mipLevel = log2(diameter / voxelWorldSize);
        // convert position to texture coord
        vec3 coord = WorldToVoxel(conePosition);
        // get directional sample from anisotropic representation
        //vec4 anisoSample = AnistropicSample(coord, weight, visibleFace, mipLevel);

		//legacy naming convention.  not actually anisotropic
		vec4 anisoSample = textureLod(volumetex, coord, mipLevel);
		vec4 indexSample = textureLod(indextex,coord,mipLevel);

		if(indexSample.r == sampleIndex.r)
			break;

        // hard shadows exit as soon cone hits something
        if(hardShadows && anisoSample.a > EPSILON) { return 0.0f; }  
        // accumulate
        visibility += (1.0f - visibility) * anisoSample.a * k;
        // move further into volume
        dst += diameter * samplingFactor;
    }

    return 1.0f - visibility;
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

		void main()
		{             
			vec4 diff = texture2DRect(difftex,gl_FragCoord.xy);
			vec3 worldray = eyepos + normalize(viewDirection)*instanceID;
			vec4 pos = texture2DRect(postex,gl_FragCoord.xy);
			//outColour = vec4(1,0,0,1);
			if(diff.a > 0.0)
			{
		
				vec4 normal = texture2DRect(normtex,gl_FragCoord.xy);
				//vec3 viewRay = vec3(viewDirection.xy * (farplane/viewDirection.z),farplane);
				//vec3 depth_pos = eyepos+viewRay*normal.a;
				vec3 depth_pos = eyepos + (normalize(viewDirection)*normal.a);
				//vec3 depth_pos = pos.xyz;
				vec4 uv = texture2DRect(uvtex,gl_FragCoord.xy);
				vec3 lightpos = vec3(-100.0,200.0,-100.0);
				vec3 lightcolour = vec3(1.0);

				if(WithinVolume(pos.xyz,256.0))
				{
					vec4 voxel = GetVoxel(pos.xyz);
					if(voxel.a > 0)
					{
						
						//vec3 snear = vec3(0);
						//vec3 sfar = vec3(0);
						//vec3 hitpoint;
						vec4 hitpoint;
						//vec4 opac = RayMarchShadow(pos.xyz,normalize(pos.xyz - lightpos),hitpoint,snear,sfar);
						float dist;
						float shade = TraceShadow2(pos.xyz, normalize(lightpos - pos.xyz),distance(lightpos,pos.xyz ),dist,hitpoint);
						//float alpha = clamp(0.5 - (shade),0.0,1.0);
						diff *= shade;
					}
				}
				outColour = diff;
				//outColour = vec4(normal.xyz,1.0);
				outColour	= vec4(GetAmbient(lightcolour,diff.xyz,normal.xyz,lightpos,pos.xyz),1.0);
				//outColour = vec4(GetAmbient(lightcolour,diff.xyz,normal.xyz,lightpos,depth_pos),1.0);
				
//if(normal.a == 0.0)
outColour = vec4(pos.w,pos.w,pos.w,1);
			}
		}  