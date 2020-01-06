#pragma once
#include "NativeHeader.h"
#include <assimp/scene.h>
//#include <assimp\Exporter.hpp>
#include "AnimationHelper.h"
#include "Serializer.h"

nativenamespace
{
	class AnimNode
	{
	public:

		AnimNode()
		{
			name = "";
			transform = mat4(1.f);
			parent = NULL;
		}

		AnimNode(const AnimNode& other)
		{
			name = other.name;
			transform = other.transform;
			parent = other.parent;
			children = other.children;
		}

		string name;
		mat4 transform;
		AnimNode* parent = NULL;
		std::vector<AnimNode*> children;
	};

	class PositionKey
	{
	public:

		PositionKey() {}
		PositionKey(const vec4& vec)
		{
			position = vec3(vec);
			time = vec.w;
		}

		vec3 position;
		double time;
	};

	class RotationKey
	{
	public:

		RotationKey() {}
		RotationKey(const quat& q)
		{
			rotation = q;
		}

		quat rotation;
		double time;

		mat4 GetAsMatrix()
		{
			return glm::toMat4(rotation);
		}
	};

	class ScaleKey
	{
	public:
		ScaleKey() {}
		ScaleKey(const vec4& vec)
		{
			scale = vec3(vec);
			time = vec.w;
		}

		vec3 scale;
		double time;
	};

	class Channel
	{
	public:

		string name;
		mat4 globalInverseTransform;
		std::vector<PositionKey> positionKeys;
		std::vector<RotationKey> rotationKeys;
		std::vector<ScaleKey> scaleKeys;
	};

	class AnimationData
	{
	public:
		AnimationData()
		{
		}

		double duration = 0.0;
		double trimStart = 0.0;
		double trimEnd = 0.0;
		string name;
		string sourceFile;
		mat4 globalInverseTransform;
		std::unordered_map<string, Channel> channels;
		std::unordered_map<string, AnimNode*> animNodes;
		AnimNode* root;
		aiAnimation* ai_animation;
		//aiScene* ai_scene;
		Save save;
		bool loaded;

		void Init(const string& animName,const string& filePath, const aiScene* aiscene)
		{
			aiAnimation* aia = aiscene->mAnimations[0];
			ai_animation = aia;
			duration = aia->mDuration;
			trimEnd = aia->mDuration;
			sourceFile = filePath;
			//ai_scene = aiscene;
			name = animName;
			BuildNodeHeirachy(aiscene->mRootNode);
			globalInverseTransform = mat4(1.f);
			GetAnimationChannels(aia, channels);
			loaded = true;
		}

		void BuildNodeHeirachy(aiNode* rootNode)
		{
			root = recursiveNodeProcess(rootNode, NULL);
		}

		AnimNode* recursiveNodeProcess(aiNode* node,AnimNode* parent)
		{
			AnimNode* n = CreateNode(node);
			n->parent = parent;
			animNodes.insert(std::make_pair(n->name, n));
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				AnimNode* c = recursiveNodeProcess(node->mChildren[i], n);
				n->children.push_back(c);
			}
			return n;
		}

		AnimNode* CreateNode(aiNode* node)
		{
			AnimNode* n = new AnimNode();
			n->name = string(node->mName.C_Str());
			n->transform = AiToGLMMat4(node->mTransformation);
			return n;
		}

		AnimNode* GetNode(string name)
		{
			return animNodes[name];
		}
		
		void GetAnimationChannels(const aiAnimation* anim, std::unordered_map<string,Channel>& channelList)
		{
			if (anim == NULL)
				return;
			duration = anim->mDuration;
			size_t numChannels = anim->mNumChannels;
			channelList.clear();
			std::vector<const aiNodeAnim*> _aiChannels;
			for (size_t i = 0; i < numChannels; ++i)
			{
				const aiNodeAnim* aiChannel = anim->mChannels[i];
				_aiChannels.push_back(aiChannel);

				Channel channel;
				ConvertChannel(aiChannel, channel);
				channels.insert(std::make_pair(channel.name, channel));
			}
			bool breakHere = true;
		}

		void ConvertChannel(const aiNodeAnim* aiChannel,Channel& channel)
		{
			channel.name = aiChannel->mNodeName.C_Str();
			GetAllPositionKeys(aiChannel, channel.positionKeys);
			GetAllRotationKeys(aiChannel, channel.rotationKeys);
			GetAllScaleKeys(aiChannel, channel.scaleKeys);
		}

		void UpdateKeys(double time)
		{
			unsigned int count = channels.size();
			//animationFrames.clear();
			for (auto& channel : channels)
			{
				AnimNode* node = GetNode(channel.second.name);
				if (node == NULL)
					continue;
				//AnimationFrame af = GetAnimationKey(channel.second, time);
				//animationFrames.insert(std::make_pair(af.name, af));
				vec3 position = BlendPosition(channel.second, time);
				quat rotation = BlendRotation(channel.second, time);
				vec3 scale = BlendScale(channel.second, time);
				mat4 mats = glm::scale(mat4(1.f), scale);
				mat4 matr = glm::toMat4(rotation);
				mat4 matt = glm::translate(mat4(1.f),position);
				node->transform = matt * matr * mats;
				//node->transform = glm::translate(mat4(1.f), position) * glm::toMat4(rotation);
				//node->transform = af.transform;
			}
		}

		
		//rename to Load, not get
		void GetAllPositionKeys(const aiNodeAnim* channel, std::vector<PositionKey>& positionKeys)
		{
			size_t count = channel->mNumPositionKeys;
			positionKeys.clear();
			positionKeys.resize(count);
			for (size_t i = 0; i < count; i++)
			{
				positionKeys[i] = GetPositionKey(channel, i);
			}
		}
		void GetAllRotationKeys(const aiNodeAnim* channel, std::vector<RotationKey>& rotationKeys)
		{
			size_t count = channel->mNumRotationKeys;
			rotationKeys.clear();
			rotationKeys.resize(count);
			for (size_t i = 0; i < count; i++)
			{
				rotationKeys[i] = GetRotationKey(channel, i);
			}
		}
		void GetAllScaleKeys(const aiNodeAnim* channel, std::vector<ScaleKey>& scaleKeys)
		{
			size_t count = channel->mNumScalingKeys;
			scaleKeys.clear();
			scaleKeys.resize(count);
			for (size_t i = 0; i < count; i++)
			{
				scaleKeys[i] = GetScaleKey(channel, i);
			}
		}

		RotationKey GetRotationKey(const aiNodeAnim* channel, size_t rotIndex)
		{
			aiQuaternion curRotation = channel->mRotationKeys[rotIndex].mValue;
			RotationKey rk;
			rk.rotation = quat(curRotation.w, curRotation.x, curRotation.y, curRotation.z);
			rk.time = channel->mRotationKeys[rotIndex].mTime;
			return rk;
		}

		PositionKey GetPositionKey(const aiNodeAnim* channel, size_t posIndex)
		{
			const aiVector3D& startpos = channel->mPositionKeys[posIndex].mValue;
			PositionKey pk;
			pk.position = vec3(startpos.x, startpos.y, startpos.z);
			pk.time = channel->mPositionKeys[posIndex].mTime;
			return pk;
		}

		ScaleKey GetScaleKey(const aiNodeAnim* channel, size_t scaleIndex)
		{
			const aiVector3D& startscale = channel->mScalingKeys[scaleIndex].mValue;
			ScaleKey sk;
			sk.scale = vec3(startscale.x, startscale.y, startscale.z);
			sk.time = channel->mScalingKeys[scaleIndex].mTime;
			return sk;
		}

		vec3 BlendPosition(const Channel& channel, double currentTime)
		{
			size_t posIndex = 0;
			size_t numKeys = channel.positionKeys.size();
			vec3 curPosition = vec3(0.f);
			while (1)
			{
				// break if this is the last key - there are no more keys after this one, we need to use it
				if (posIndex + 1 >= numKeys)
					break;
				// break if the next key lies in the future - the current one is the correct one then
				if (channel.positionKeys[posIndex + 1].time > currentTime)
					break;
				posIndex += 1;
			}

			unsigned int nextPositionIndex = (posIndex + 1);
			if (nextPositionIndex >= numKeys)
			{
				curPosition = channel.positionKeys[posIndex].position;
			}
			else {
				assert(nextPositionIndex < numKeys);
				double deltaTime = channel.positionKeys[nextPositionIndex].time - channel.positionKeys[posIndex].time;
				float factor = (float)(currentTime - channel.positionKeys[posIndex].time / deltaTime);
				//seems like when the our default landing jump animtion loops factor becomes negative
				if (factor < 0.f)
					factor = 0.f;
				assert(factor >= 0.0f && factor <= 1.0f);
				const vec3& startPos = channel.positionKeys[posIndex].position;
				const vec3& endPos = channel.positionKeys[nextPositionIndex].position;
				vec3 sp = vec3(startPos.x, startPos.y, startPos.z);
				vec3 ep = vec3(endPos.x, endPos.y, endPos.z);
				curPosition = glm::mix(sp, ep, factor);
			}
			return curPosition;
		}

		quat BlendRotation(const Channel& channel, double currentTime)
		{
			size_t rotIndex = 0;
			size_t numRotKeys = channel.rotationKeys.size();
			quat curRotation = quat(1.f, 0.f, 0.f, 0.f);

			while (1)
			{
				if (rotIndex + 1 >= numRotKeys)
					break;
				if (channel.rotationKeys[rotIndex + 1].time > currentTime)
					break;
				rotIndex += 1;
			}

			unsigned int nextRotationIndex = (rotIndex + 1);
			if (nextRotationIndex >= numRotKeys)
			{
				curRotation = channel.rotationKeys[rotIndex].rotation;
			}
			else {
				assert(nextRotationIndex < numRotKeys);
				double deltaTime = channel.rotationKeys[nextRotationIndex].time - channel.rotationKeys[rotIndex].time;
				float factor = (float)(currentTime - channel.rotationKeys[rotIndex].time / deltaTime);
				if (factor < 0.f)
					factor = 0.f;
				assert(factor >= 0.0f && factor <= 1.0f);
				const quat& startRotationQ = channel.rotationKeys[rotIndex].rotation;
				const quat& endRotationQ = channel.rotationKeys[nextRotationIndex].rotation;
				curRotation = glm::lerp(startRotationQ, endRotationQ, factor);
				curRotation = glm::normalize(curRotation);
			}
			return curRotation;
		}

		vec3 BlendScale(const Channel& channel, double currentTime)
		{
			size_t scaleIndex = 0;
			size_t numKeys = channel.scaleKeys.size();
			vec3 curScale = vec3(0.f);
			
			while (1)
			{
				// break if this is the last key - there are no more keys after this one, we need to use it
				if (scaleIndex + 1 >= numKeys)
					break;
				// break if the next key lies in the future - the current one is the correct one then
				if (channel.scaleKeys[scaleIndex + 1].time > currentTime)
					break;
				scaleIndex += 1;
			}
			unsigned int nextScaleIndex = (scaleIndex + 1);
			if (nextScaleIndex >= numKeys)
			{
				curScale = channel.scaleKeys[scaleIndex].scale;
			}
			else {
				assert(nextScaleIndex < numKeys);
				double deltaTime = channel.scaleKeys[nextScaleIndex].time - channel.scaleKeys[scaleIndex].time;
				float factor = (float)(currentTime - channel.scaleKeys[scaleIndex].time / deltaTime);
				//seems like when the our default landing jump animtion loops factor becomes negative
				if (factor < 0.f)
					factor = 0.f;
				assert(factor >= 0.0f && factor <= 1.0f);
				const vec3& startScale = channel.scaleKeys[scaleIndex].scale;
				const vec3& endScale = channel.scaleKeys[nextScaleIndex].scale;
				curScale = glm::mix(startScale, endScale, factor);
			}
			return curScale;
		}



		RotationKey* GetRotationKey(const string& name, double currentTime)
		{
			Channel* c = &channels[name];
			if (c == NULL)
				return NULL;

			size_t rotIndex = 0;
			size_t numRotKeys = c->rotationKeys.size();

			while (1)
			{
				if (rotIndex + 1 >= numRotKeys)
					break;
				if (c->rotationKeys[rotIndex + 1].time > currentTime)
					break;
				rotIndex += 1;
			}
			return &c->rotationKeys[rotIndex];
		}

		void LoadAnim()
		{
			LoadAnim(save.file);
		}

		void LoadAnim(const string& filePath)
		{
			save.file = filePath;

			//std::vector<string> channelnames = save.GetStrings("Channel::");
			std::vector<string> allLines;
			cout << "\nAnimation Data: Importing file: " << filePath << endl;
			if (!save.GetAllLines(allLines))
			{
				cout << ">>>>>>>ERROR!<<<<<<< AnimationData failed to import file"<< endl;
				return;
			}

			std::vector<string> getheader;
			Save::DelimitSplit("::Animation Name::", allLines[0], getheader);
			if (getheader.size() == 1)
				name = getheader[0];

			getheader.clear();
			Save::DelimitLine(name + ":Variables:", ">>", allLines[1], getheader);
			trimStart = Save::ExtractAsDouble(getheader[0]);
			trimEnd = Save::ExtractAsDouble(getheader[1]);
			duration = Save::ExtractAsDouble(getheader[2]);

			getheader.clear();
			Save::DelimitLine("::Source File::", ">>", allLines[2], getheader);
			if(getheader.size() == 1)
				sourceFile = getheader[0];

			channels.clear();
			animNodes.clear();
			std::unordered_map<string, AnimNode*>::iterator nodeSearch;
			std::unordered_map<string, string> nodeparenting;
			int count = 0;
			for (string line : allLines)
			{
				std::vector<string> output;
				//position
				Save::DelimitSplit("::PositionKeys Size::", line, output);
				if (output.size() == 2)
				{
					//allocate  position keys list
					//channels.insert(std::pair())
					Channel& channel = channels[output[0]];
					channel.name = output[0];
					channel.positionKeys.clear();
					int size = Save::ExtractAsInt(output[1]);
					channel.positionKeys.resize(size);
					count++;
					continue;
				}
				output.clear();
				Save::DelimitSplit(">>PositionChannel>>", line,output);
				if (output.size() == 2)
				{
					string channelName = output[0];
					//cout << "Channel Name: " << channelName << endl;
					string result = output[1];
					output.clear();
					Save::DelimitSplit(">>type>>", result, output);
					string keyType = output[0];
					//cout << "key Type: " << keyType << endl;
					result = output[1];
					output.clear();
					Save::DelimitSplit(">>index>>", result, output);
					string index = output[0];
					int posIndex = Save::ExtractAsInt(index);
					//cout << "Index: " << index << endl;
					result = output[1];
					output.clear();
					Save::DelimitSplit("||", result, output);
					vec4 vec = Save::ExtractVec4(output);
					//cout << "vec4: " << glmGetString(vec) << endl;
					Channel& channel = channels[channelName];
					channel.positionKeys[posIndex] = vec;
					count++;
					continue;
				}
				
				//rotation
				output.clear();
				Save::DelimitSplit("::RotationKeys Size::", line, output);
				if (output.size() == 2)
				{
					//allocate  position keys list
					Channel& channel = channels[output[0]];
					channel.name = output[0];
					channel.rotationKeys.clear();
					int size = Save::ExtractAsInt(output[1]);
					channel.rotationKeys.resize(size);
					count++;
					continue;
				}
				output.clear();
				Save::DelimitSplit(">>RotationChannel>>", line, output);
				if (output.size() == 2)
				{
					string channelName = output[0];
					//cout << "Channel Name: " << channelName << endl;
					string result = output[1];
					output.clear();
					Save::DelimitSplit(">>type>>", result, output);
					string keyType = output[0];
					//cout << "key Type: " << keyType << endl;
					result = output[1];
					output.clear();
					Save::DelimitSplit(">>index>>", result, output);
					string index = output[0];
					int rotIndex = Save::ExtractAsInt(index);
					//cout << "Index: " << index << endl;
					result = output[1];
					output.clear();
					Save::DelimitSplit("||", result, output);
					vec4 vec = Save::ExtractVec4(output);
					quat q = quat(vec.w, vec.x, vec.y, vec.z);
					//cout << "vec4: " << glmGetString(vec) << endl;
					Channel& channel = channels[channelName];
					channel.rotationKeys[rotIndex] = q;
					channel.rotationKeys[rotIndex].time = Save::ExtractAsDouble(output[4]);
					count++;
					continue;
				}

				//Scale
				output.clear();
				Save::DelimitSplit("::ScaleKeys Size::", line, output);
				if (output.size() == 2)
				{
					//allocate  position keys list
					Channel& channel = channels[output[0]];
					channel.name = output[0];
					channel.scaleKeys.clear();
					int size = Save::ExtractAsInt(output[1]);
					channel.scaleKeys.resize(size);
					count++;
					continue;
				}
				output.clear();
				Save::DelimitSplit(">>ScaleChannel>>", line, output);
				if (output.size() == 2)
				{
					string channelName = output[0];
					//cout << "Channel Name: " << channelName << endl;
					string result = output[1];
					output.clear();
					Save::DelimitSplit(">>type>>", result, output);
					string keyType = output[0];
					//cout << "key Type: " << keyType << endl;
					result = output[1];
					output.clear();
					Save::DelimitSplit(">>index>>", result, output);
					string index = output[0];
					int scaleIndex = Save::ExtractAsInt(index);
					//cout << "Index: " << index << endl;
					result = output[1];
					output.clear();
					Save::DelimitSplit("||", result, output);
					vec4 vec = Save::ExtractVec4(output);
					//cout << "vec4: " << glmGetString(vec) << endl;
					Channel& channel = channels[channelName];
					channel.scaleKeys[scaleIndex] = vec;
					count++;
					continue;
				}
				
				//nodes
				output.clear();
				Save::DelimitLine(name + "::Node Name::", ">>", line,output);
				if (output.size() > 0)
				{
					assert(output.size() <= 2);
					nodeSearch = animNodes.find(output[0]);
					AnimNode* an = new AnimNode();
					an->name = output[0];
					string mline = allLines[count + 1];
					an->transform = Save::ExtractAsMat4("||", mline);
					animNodes.insert(std::make_pair(output[0], an));
					nodeparenting.insert(std::make_pair(output[0], output[1]));
				}
				count++;
			}

			for (auto pair : animNodes)
			{
				AnimNode* an = pair.second;
				string parentname = nodeparenting[an->name];
				if (parentname == "null")
					an->parent = NULL;
				else
					an->parent = animNodes[parentname];
			}
			loaded = true;
			cout << "Import finished" << endl;
		}

		void SaveAnim()
		{
			string file = "Assets\\" + name + ".glax";
			SaveAnim(name,file);
		}

		void SaveAnim(const string& animName, const string& filePath)
		{
			cout << "Exporting animation file: " << filePath << "..." << endl;
			
			save.CreateTextFile(filePath);
			
			std::ofstream stream = save.EnableFileStream();
			if (!stream.good())
			{
				cout << ">>>>>>ERROR!<<<<<<< Failed to export: unable to open file" << endl;
				return;
			}
			name = animName;
			string variableheader = name + ":Variables:" + ">>" + to_string(trimStart) + ">>" + to_string(trimEnd) + ">>" + to_string(duration) + ">>";
			save.AddLineToStream(stream, "::Animation Name::" + name);
			save.AddLineToStream(stream, variableheader);
			save.AddLineToStream(stream, name + "::Source File::" + sourceFile);
			for (auto& pair : channels)
			{
				Channel& c = pair.second;
				
				string cn = pair.second.name;
				//positions
				int pksize = c.positionKeys.size();
				save.AddLineToStream(stream,cn + "::PositionKeys Size::"+ to_string(pksize));
				for (int i=0;i<pksize;i++)
				{
					string field = cn + ">>PositionChannel>>PositionKey>>type>>" + to_string(i) + ">>index>>";
					PositionKey pk = c.positionKeys[i];
					string _vl = field + Save::MakeVec4Line("||",vec4(pk.position.x, pk.position.y, pk.position.z, pk.time));
					save.AddLineToStream(stream,_vl);
				}
				//rotations
				
				int rksize = c.rotationKeys.size();
				save.AddLineToStream(stream, cn + "::RotationKeys Size::" + to_string(rksize));
				for (int i = 0; i<rksize; i++)
				{
					string field = cn + ">>RotationChannel>>RotationKey>>type>>" + to_string(i) + ">>index>>";
					RotationKey rk = c.rotationKeys[i];
					string _vl = field + Save::MakeVec4Line("||", vec4(rk.rotation.x, rk.rotation.y, rk.rotation.z, rk.rotation.w)) + to_string(rk.time) + "||";
					save.AddLineToStream(stream, _vl);
				}
				//scale
				int sksize = c.scaleKeys.size();
				save.AddLineToStream(stream, cn + "::ScaleKeys Size::" + to_string(sksize));
				for (int i = 0; i<sksize; i++)
				{
					string field = cn + ">>ScaleChannel>>ScaleKey>>type>>" + to_string(i) + ">>index>>";
					ScaleKey sk = c.scaleKeys[i];
					string _vl = field + Save::MakeVec4Line("||", vec4(sk.scale.x, sk.scale.y, sk.scale.z, sk.time));
					save.AddLineToStream(stream, _vl);
				}
			}

			for (auto& pair : animNodes)
			{
				string nodename = pair.second->name;
				string parentname = "null";
				if (pair.second->parent != NULL)
					parentname = pair.second->parent->name;
				string _snode = name + "::Node Name::>>" + nodename + ">>" + parentname + ">>";
				string _snodematrix = Save::MakeMat4Line("||", pair.second->transform);
				save.AddLineToStream(stream, _snode);
				save.AddLineToStream(stream, _snodematrix);
			}

			save.CloseFileStream(stream);
			cout << "Export Finished " << endl;
		}

		void CleanUp()
		{
			for (auto an : animNodes)
				delete an.second;
			animNodes.clear();
		}
	};
}

/*
//////////////////////////////////////////////////////////////////////Export
void ExportAnimationScene()
{
ExportAnimation(ai_animation);
}

void ExportAnimation(aiAnimation* anim)
{
if (anim == NULL)
return;
//anim->mDuration = totalDuration;
size_t numChannels = anim->mNumChannels;
for (size_t i = 0; i < numChannels; ++i)
{
aiNodeAnim* aiChannel = anim->mChannels[i];

Channel& channel = channels[aiChannel->mNodeName.C_Str()];
ConvertToAiChannel(aiChannel, channel);
}
bool breakHere = true;
}
void ConvertToAiChannel(aiNodeAnim* aiChannel, Channel& channel)
{
//channel.name = aiChannel->mNodeName.C_Str();
//aiChannel->mNodeName = aiString(channel.name);

int numPosKeys = channel.positionKeys.size();
for (int i = 0; i < numPosKeys; i++)
SetPositionKeys(aiChannel,channel,i);

int numRotKeys = channel.rotationKeys.size();
for (int i = 0; i < numRotKeys; i++)
SetRotationKeys(aiChannel, channel, i);

int numScaleKeys = channel.scaleKeys.size();
for (int i = 0; i < numScaleKeys; i++)
SetScaleKeys(aiChannel, channel, i);
}

void SetPositionKeys(aiNodeAnim* aichannel,Channel c, size_t index)
{
PositionKey pk = c.positionKeys[index];
aiVector3D pos = aiVector3D(pk.position.x, pk.position.y, pk.position.z);
aichannel->mPositionKeys[index].mValue = pos;
//aichannel->mPositionKeys[posIndex].mTime = pk.time;
}
void SetRotationKeys(aiNodeAnim* aichannel, Channel c, size_t index)
{
RotationKey rk = c.rotationKeys[index];
aiQuaternion rot = aiQuaternion(rk.rotation.w, rk.rotation.x, rk.rotation.y, rk.rotation.z);
aichannel->mRotationKeys[index].mValue = rot;

//aichannel->mRotationKeys[posIndex].mTime = rk.time;
}
void SetScaleKeys(aiNodeAnim* aichannel, Channel c, size_t index)
{
ScaleKey sk = c.scaleKeys[index];
aiVector3D scale = aiVector3D(sk.scale.x, sk.scale.y, sk.scale.z);
aichannel->mScalingKeys[index].mValue = scale;
//channel->mScalingKeys[posIndex].mTime = sk.time;
}

*/
