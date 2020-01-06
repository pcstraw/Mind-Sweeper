#version 440

out vec4 outcolour;
layout(binding=1) uniform sampler2D tex2d;

void main()
{
	//vec4 s = texelFetch(tex2d,samplepos,0);
	ivec2 size = textureSize(tex2d,0);
	vec4 s = texture(tex2d,gl_FragCoord.xy/size);
	if(s.a < 0.0)
		discard;

	vec4 col = s;
	if(!gl_FrontFacing)
		col = vec4(1.0,0.0,0.0,1.0);
	outcolour = col;
}