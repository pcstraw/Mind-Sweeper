#version 440
			
//layout (location = 0) in vec2 Position; 
	
layout(binding=1) uniform sampler2DRect postex;
layout(binding=2) uniform sampler2DRect normtex;
layout(binding=3) uniform sampler2DRect difftex;
layout(binding=4) uniform sampler2DRect shadowtex;

layout(binding=5) uniform sampler3D volumetex;
layout(binding=6) uniform isampler3D indextex;
layout(binding=7, rgba16f) uniform image3D voltex;

uniform sampler2DArray textureArray;

uniform vec2 screensize;
uniform vec3 eyepos;
uniform float farplane;
const float EPSILON = 1e-30;
uniform float samplingFactor = 0.5f;

in vec3 viewDirection;
in flat int instanceID;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 lightpos;

layout (location = 0) out vec4 outColour;

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

void main()
{   
	vec2 coord = vec2(gl_FragCoord.xy/screensize);
	
	vec4 indiff = texture(textureArray,vec3(coord,2));
	if(indiff.a > 0.0)
	{
		vec4 pos = texture(textureArray,vec3(coord,0));
		vec4 normal = texture(textureArray,vec3(coord,1));
		vec3 depth_pos = eyepos + (normalize(viewDirection)*normal.a);
			
		vec3 lightcolour = vec3(1.0);
		//outColour = vec4(normal.xyz + vec3(0.5),1);
		vec4 finalColor =  vec4(GetAmbient(lightcolour,indiff.xyz,normal.xyz,lightpos,depth_pos),1.0);
		/*
		const float LOG2 = 1.442695;
		float density = 0.001;
		vec4 fogColour = vec4(1.0,0,0,1);
		float z = normal.a;
		float fogFactor = exp2( -density * 
						   density * 
						   z * 
						   z * 
						   LOG2 );
		fogFactor = clamp(fogFactor, 0.0, 1.0);
		outColour = mix(fogColour, finalColor, fogFactor );
		*/
		outColour = finalColor;
		
		//outColour = vec4(normal.xyz,2.0);
	}else
		discard;
}  