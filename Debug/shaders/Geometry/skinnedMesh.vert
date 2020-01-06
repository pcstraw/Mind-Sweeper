#version 440
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout(location = 6) in vec3 inVertexNormal;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];
uniform mat4 projection;
uniform mat4 model;
uniform mat4 viewmodel;
uniform mat4 view;

out vData
{
vec4 pos;
vec4 normal;
vec3 uv;
vec4 weights;
vec3 vertexNormal;
vec3 cameraRelPos;
int instance;
}vertex;

void main(void)
{
	mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
			BoneTransform += gBones[BoneIDs[1]] * Weights[1];
			BoneTransform += gBones[BoneIDs[2]] * Weights[2];
			BoneTransform += gBones[BoneIDs[3]] * Weights[3];
	vec3 inpos = inPosition;
	mat4 trans = model;
	mat4 transformedBone = trans*BoneTransform;
	vec4 pos = projection * transformedBone*vec4(inpos,1.0);
	gl_Position = pos;
			
	vertex.pos = transformedBone * vec4(inpos,1.0);
	vec4 NormalL = vec4(inNormal,0.0);
	vertex.normal =  (transpose(inverse(transformedBone))*NormalL);
	vertex.vertexNormal =  (transpose(inverse(transformedBone))*vec4(inVertexNormal,0)).xyz;
	vertex.uv = vec3(inUV,0.0);
	vertex.weights = Weights;
	vertex.cameraRelPos = (viewmodel*BoneTransform*vec4(inpos,1.0)).xyz;
	vertex.instance = gl_InstanceID;
}