#pragma once
#include "NativeHeader.h"
#include "GPUData.h"
#include "Serializer.h"

nativenamespace  {

	class Vertex
	{
	private:

	public:

		Vertex()
		{
			hash = -1;
		}

		Vertex(const glm::vec3& pos)
		{
			position = pos;
			hash = -1;
		}

		Vertex(const vec3& Position, const vec2& UV, const vec3& Normal,int vertexID)
		{
			position = Position;
			uv = UV;
			faceNormal = Normal;
			colour = vec4(1.f);
			weights = vec4(0.f);
			hasWeights = false;
			boneIDs = uvec4(0);
			index = vertexID;
			hash = -1;
		}

		Vertex(const Vertex& v)
		{
			position = v.position;
			uv = v.uv;
			faceNormal = v.faceNormal;
			colour = v.colour;
			weights = v.weights;
			boneIDs = v.boneIDs;
			hasWeights = v.hasWeights;
			vertexNormal = v.vertexNormal;
			index = v.index;
			hash = v.hash;
			weightedNormal = v.weightedNormal;
		}

		void AddBoneWeight(int boneIndex, float weight)
		{
			for (int i = 0; i < 4; i++)
			{
				if (weights[i] == 0.f)
				{
					weights[i] = weight;
					boneIDs[i] = boneIndex;
					hasWeights = true;
					return;
				}
			}
			//should enever reach this code
			assert(0);
		}

		vec3 position;
		vec2 uv;
		vec3 faceNormal; //facenormal
		vec3 vertexNormal;
		vec3 weightedNormal;
		vec4 colour;
		vec4 weights;
		uvec4 boneIDs;
		bool hasWeights;
		int index;
		int hash;

		static std::vector<vec3> GetPositions(const std::vector<Vertex> vertices)
		{
			std::vector<vec3> list;
			for (const Vertex& v : vertices)
				list.push_back(v.position);
			return list;
		}

		static std::vector<vec3> GetNormals(const std::vector<Vertex> vertices)
		{
			std::vector<vec3> list;
			for (const Vertex& v : vertices)
				list.push_back(v.faceNormal);
			return list;
		}

		static std::vector<vec2> GetUVs(const std::vector<Vertex> vertices)
		{
			std::vector<vec2> list;
			for (const Vertex& v : vertices)
				list.push_back(v.uv);
			return list;
		}

		static std::vector<uvec4> GetBoneIDs(const std::vector<Vertex> vertices)
		{
			std::vector<uvec4> list;
			for (const Vertex& v : vertices)
				list.push_back(v.boneIDs);
			return list;
		}

		static std::vector<vec4> GetBoneWeights(const std::vector<Vertex> vertices)
		{
			std::vector<vec4> list;
			for (const Vertex& v : vertices)
				list.push_back(v.weights);
			return list;
		}

		static std::vector<ArrayBuffer> GetArrayBuffer(const std::vector<Vertex> vertices,bool hasWeights = false)
		{
			std::vector<ArrayBuffer> list;
			std::vector<float> pos;
			std::vector<float> normals;
			std::vector<float> uvs;
			std::vector<float> weights;
			std::vector<unsigned int> boneIDs;
			for (const Vertex& v : vertices)
			{
				pos.push_back(v.position.x);
				pos.push_back(v.position.y);
				pos.push_back(v.position.z);
				normals.push_back(v.faceNormal.x);
				normals.push_back(v.faceNormal.y);
				normals.push_back(v.faceNormal.z);
				uvs.push_back(v.uv.x);
				uvs.push_back(v.uv.y);
				if (!hasWeights)
					continue;
				weights.push_back(v.weights.x);
				weights.push_back(v.weights.y);
				weights.push_back(v.weights.z);
				weights.push_back(v.weights.w);
				boneIDs.push_back(v.boneIDs.x);
				boneIDs.push_back(v.boneIDs.y);
				boneIDs.push_back(v.boneIDs.z);
				boneIDs.push_back(v.boneIDs.w);
			}

			list.push_back(ArrayBuffer(pos));
			list[0].SetDataType(BufferDataType::Float3);
			list.push_back(ArrayBuffer(normals));
			list[1].SetDataType(BufferDataType::Float3);
			list.push_back(ArrayBuffer(uvs));
			list[2].SetDataType(BufferDataType::Float2);
			if (hasWeights)
			{
				list.push_back(ArrayBuffer(boneIDs));
				list[3].SetDataType(BufferDataType::Int4);
				list.push_back(ArrayBuffer(weights));
				list[4].SetDataType(BufferDataType::Float4);
			}

			return list;
		}

		int HashVertex()
		{
			int a = position.x * 1000;
			int b = position.y * 1000;
			int c = position.z * 1000;
			int P1 = 31;
			int P2 = 53;
			int h = (a*P1 + b)*P2 + c;
			hash = h;
			return h;
		}

		void PrintHash()
		{
			cout << "Vertex: " << endl;
			cout << ">> X: " << position.x << endl;
			cout << ">> Y: " << position.y << endl;
			cout << ">> Z: " << position.z << endl;
			cout << ">> Hash: "<< hash << "\n"<<endl;
		}

		void Deserialize(const string& line)
		{
			string remainder = "";
			string result = Save::SplitLine(":position:", line, remainder);
			vec3 _pos = Save::ExtractAsVec3("||", result);

			result = Save::SplitLine(":uv:", remainder, remainder);
			vec2 _uvs = Save::ExtractAsVec2("||", result);

			result = Save::SplitLine(":normal:", remainder, remainder);
			vec3 _normal = Save::ExtractAsVec3("||", result);

			result = Save::SplitLine(":weights:", remainder, remainder);
			vec4 _weights = Save::ExtractAsVec4("||", result);

			result = Save::SplitLine(":bones:", remainder, remainder);
			uvec4 _bones = Save::ExtractAsUVec4("||", result);
		}

		string SerializeString(Save& save,int index)
		{
			//ss << Save::SerializeVec3(":position:", position); << Save::SerializeVec2(":uv:", uv) << Save::SerializeVec3(":normal:", faceNormal) << Save::SerializeVec4(":weights:", weights) << Save::SerializeUVec4(":bones:", boneIDs);
			save.AddLine(Save::SerializeVec3(position));
			save.AddLine(Save::SerializeVec2(uv));
			save.AddLine(Save::SerializeVec3(faceNormal));
			save.AddLine(Save::SerializeVec4(weights));
			save.AddLine(Save::SerializeUVec4(boneIDs));

			return "";
		}

		~Vertex() {}
	};
}