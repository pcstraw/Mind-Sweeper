#pragma once
#include "Bone.h"
#include <assimp/scene.h>
#include "AnimationData.h"

nativenamespace
{
	class Skeleton
	{

	public:
		std::vector<Bone*> bones;
		std::map<string, int> boneMap;
		std::vector<mat4> boneMatrices;
		bool debug;

		Skeleton()
		{
			debug = false;
		}

		void Init(const aiMesh* aimesh)
		{
			bones.clear();

			//storing bone data
			for (int j = 0; j < aimesh->mNumBones; j++)
			{
				const aiBone* b = aimesh->mBones[j];

				string b_name = b->mName.data;
				
				mat4 b_mat = AiToGLMMat4(b->mOffsetMatrix);

				if (debug)
					std::cout << "Bone " << j << " " << b_name << endl;

				Bone* bone = new Bone(j, b_name, b_mat);
				boneMap.insert(std::make_pair(b_name, j));
				bones.push_back(bone);
			}

			boneMatrices.resize(100);
			for (int i = 0; i < 100; i++)
				boneMatrices[i] = mat4(1.f);
		}

		void UpdateBones(AnimationData& dopeSheet)
		{
			AnimateBones(dopeSheet, *this, boneMatrices);
		}

		static void AnimateBones(AnimationData& dopeSheet, Skeleton& skeleton, std::vector<mat4>& boneList)
		{
			boneList.clear();
			boneList.resize(100); //shader bone max

			unsigned int boneCount = skeleton.bones.size();
			int index = 0;
			for (int i = 0; i<boneCount; i++)
			{
				Bone* bone = skeleton.bones[i];
				if (bone == NULL)
					continue;
				bone->parent = skeleton.GetParentBone(dopeSheet, bone);
				AnimNode* node = dopeSheet.GetNode(bone->name);
				if (node == NULL)
					continue;

				AnimNode* tempNode = node;
				mat4 m = bone->offsetMatrix;

				while (tempNode)
				{
					m = tempNode->transform*m;
					tempNode = tempNode->parent;
				}
				bone->animatedMatrix = dopeSheet.globalInverseTransform * m;
				bone->worldMatrix = bone->GetWorldMatrix();
				boneList[index] = bone->animatedMatrix;
				index++;
			}

			for (int i = index; i < 100; i++)
				boneList[i] = mat4(1.f);
		}
		
		//move to  dopesheet.  Use strings instead of bones
		Bone* GetParentBone(AnimationData& dopeSheet,Bone* bone)
		{
			AnimNode* n = dopeSheet.GetNode(bone->name);
			
			if (n == NULL||n->parent == NULL)
				return NULL;
			int parent = boneMap[n->parent->name];
			if (parent > -1)
				return bones[parent];
			else
				return NULL;
		}

		Bone* FindBoneByName(const string& name)
		{
			for (int i=0;i<bones.size();i++)
			{
				if (bones[i]->name == name)
					return bones[i];
			}
			return NULL;
		}
	};
}

/*
/////////////////////////////////////////export
void ExportBonesToBindPose()
{
for (int j = 0; j < mesh->mNumBones; j++)
{
mesh->mBones[j]->mOffsetMatrix = GLMMat4ToAi(bones[j]->offsetMatrix);
/*
//const aiBone* b = mesh->mBones[j];

string b_name = b->mName.data;

mat4 b_mat = AiToGLMMat4(b->mOffsetMatrix);

if (debug)
std::cout << "Bone " << j << " " << b_name << endl;

Bone* bone = new Bone(j, b_name, b_mat);
boneMap.insert(std::make_pair(b_name, j));
bones.push_back(bone);

			}
		}
*/