#pragma once
#include"NativeHeader.h"
#include <assimp/scene.h>
#include "AnimationData.h"
#include "GPUMesh.h"
#include "Faces.h"

nativenamespace {

#define MAX_BONES 100

	class Bone
	{
	public:
		Bone()
		{
			name = "Nameless bone";
			id = -1;
			parentID = -1;
			offsetMatrix = mat4(1.f);
			animatedMatrix = mat4(1.f);
			worldMatrix = mat4(1.f);
		}

		Bone(const Bone& other)
		{
			id = other.id;
			name = other.name;
			offsetMatrix = other.offsetMatrix;
			animatedMatrix = other.animatedMatrix;
			parent = other.parent;
			parentID = other.parentID;
		}
		
		Bone(int in_id, string in_name, aiMatrix4x4 in_o_mat)
		{
			id = in_id;
			name = in_name;
			offsetMatrix = AiToGLMMat4(in_o_mat);
		}

		Bone(int in_id, string in_name, mat4 in_o_mat)
		{
			id = in_id;
			name = in_name;
			offsetMatrix = in_o_mat;
			inverseOffsetMatrix = glm::inverse(offsetMatrix);
		}

		string name;
		int id;
		mat4 offsetMatrix;
		mat4 inverseOffsetMatrix;
		mat4 animatedMatrix; //need to rename this
		mat4 worldMatrix;
		Bone* parent = NULL;
		int parentID;

		vec3 GetWorldPosition() const
		{
			return vec3(worldMatrix[3]);
		}

		mat4 GetWorldMatrix() const
		{
			return animatedMatrix*inverseOffsetMatrix;
		}

		static GPUMesh BuildBoneMesh()
		{
			std::vector<Face> faces;

			BuildBoneVertices(faces);

			GPUMesh mesh;
			mesh.BufferMesh(faces);
			return mesh;
		}

		static void BuildBoneVertices(std::vector<Face>& faces)
		{
			Vertex tip = vec3(0, 0, 40);
			Vertex right = vec3(10, 0, 0);
			Vertex top = vec3(0, 10, 0);
			Vertex bottom = vec3(0, -10, 0);
			Vertex left = vec3(-10, 0, 0);
			Vertex back = vec3(0, 0, -10);
			//top right
			Face tr(tip, right, top);
			faces.push_back(tr);
			//bottom right
			Face br(tip, bottom, right);
			faces.push_back(br);
			//top left
			Face tl(tip, top, left);
			faces.push_back(tl);
			//bottom left
			Face bl(left, bottom,tip);
			faces.push_back(bl);
			//back right top
			Face brt(top, right, back);
			faces.push_back(brt);
			//back left top
			Face blt(back, left, top);
			faces.push_back(blt);
			//back right bottom
			Face brb(back, right, bottom);
			faces.push_back(brb);
			//back left bottom
			Face blb(bottom, left, back);
			faces.push_back(blb);
		}
	};
}
