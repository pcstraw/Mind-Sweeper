#pragma once
#include "NativeHeader.h"
#include "Vertex.h"
#include "Transform.h"
#include "GPUMesh.h"
#include "Skeleton.h"
#include "ObjectSelection.h"
#include "Faces.h"
/*
#include <concrt.h>
#include <ppl.h>
#include <algorithm>
#include <execution>
#include "amp.h"
*/

#define NUM_BONES_PER_VEREX 4

nativenamespace{

	class Mesh 
	{
	public:

		Mesh() {

			HasWeights = false;
			colour = glm::vec4(1.f);
			GenerateID();
		};

		~Mesh() {};

		vec4 colour;
		Transform transform;
		Skeleton skeleton;
		GPUMesh gpuData;
		int selectionID;
		bool HasWeights;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Face> faces;
		Save save;

		void GenerateID()
		{
			selectionID = ObjectSelection::GenerateID();
		}

		void Buffer()
		{
			gpuData.BufferMesh(vertices, indices, HasWeights);
		}

		//quickly calculate veretx normals
		void CalculateVertexNormals(std::vector<vec3>& vertexNormals)
		{
			int numVertex = vertices.size();
			vertexNormals.resize(numVertex);

			for (Face& f : faces)
			{
				vertexNormals[f.faceIndices.x] += f.normal;
				vertexNormals[f.faceIndices.y] += f.normal;
				vertexNormals[f.faceIndices.z] += f.normal;
			}

			for (int i = 0; i < numVertex; i++)
			{
				glm::vec3 n = glm::normalize(vertexNormals[i]);
				vertexNormals[i] = n;
				vertices[i].vertexNormal = n;
			}
		}

		vec3 GetLocalSupportPoint(glm::vec3 direction)
		{
			vec3 returnVector;
			vec3 localVertex = vertices[0].position;
			vec3 local_dir = direction;
			float maxDot = dot(localVertex,local_dir);
			returnVector = localVertex;

			for (int i = 1; i < vertices.size(); i++)
			{
				//stay in local coords
				localVertex = vertices[i].position;
				float d = glm::dot(localVertex,local_dir);
				//find biggest dot product
				if (d > maxDot)
				{
					maxDot = d;
					returnVector = localVertex;
				}
			}
			return returnVector;
		}

		string SerializeVertex(Vertex& v)
		{
			int index = 0;
			std::ostringstream ss;
			ss << Save::SerializeVec3(":position:", v.position) << Save::SerializeVec2(":uv:", v.uv) << Save::SerializeVec3(":normal:", v.faceNormal) << Save::SerializeVec4(":weights:", v.weights) << Save::SerializeUVec4(":bones:", v.boneIDs);
			return ss.str();
		}

		void LoadFile(string modelName,int meshIndex)
		{
			string filePath = "Assets\\" + modelName + "-" + to_string(meshIndex) + ".mesh";
			cout << "Load Mesh Save File: " << filePath << endl;
			std::vector<string> alllines;
			save.file = filePath;
			save.GetAllLines(alllines);
			int vertexCount = Save::ExtractAsInt(alllines[1]);
			vertices.clear();
			vertices.resize(vertexCount);
			int index = 0;
			int dataIndex = 0;
			int lineCount = alllines.size();
			for (int i=2;i<lineCount;i+=6)
			{
				vertices[index].position = Save::ExtractAsVec3("||",alllines[i + 0]);
				vertices[index].uv = Save::ExtractAsVec2("||", alllines[i + 1]);
				vertices[index].faceNormal = Save::ExtractAsVec3("||", alllines[i + 2]);
				vertices[index].weights = Save::ExtractAsVec4("||", alllines[i + 3]);
				vertices[index].boneIDs = Save::ExtractAsUVec4("||", alllines[i + 4]);
				index++;
			}

			Buffer();
		}

		void Save(int meshIndex,const string& name,const string& sourceFile)
		{
			string filePath = "Assets\\" + name + "-"+to_string(meshIndex)+ ".mesh";
			cout << "Creating Mesh Save File: " << filePath << endl;
			save.CreateTextFile(filePath);

			save.EnableOutStream();
			save.AddLine(sourceFile);
			save.AddLine(to_string(vertices.size()));
			
			for (int i=0;i<vertices.size();i++)
			{
				string s = vertices[i].SerializeString(save,i);
				//string s = meshIdentifier + vertices[i].Serialize(i);
				save.AddLine(s);
			}
			save.CloseOutStream();
			return;
			

			//std::for_each(vertices.begin(), vertices.end(), std::bind(&Mesh::SerializeVertex, &*this, std::placeholders::_1));

			/*
			Concurrency::parallel_for(vertices.size(), [&](int start, int end) {
				for (int i = start; i < end; ++i)
					computation(i);
			});
			*/
			//Concurrency::parallel_for_each()
		}
	};
}