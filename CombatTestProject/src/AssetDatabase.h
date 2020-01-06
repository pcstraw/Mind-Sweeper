#pragma once
#include "NativeHeader.h"
#include "AnimationData.h"
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include "Modal.h"

nativenamespace{

	class Assets
	{

	public:

		std::unordered_map<string,AnimationData*> animations;
		std::unordered_map<string, string> animationFiles;
		std::vector<Model*> models;
		Save save;

		static Assets instance;

		static Model* ImportModel(const string& filePath, bool addToDatabase = true)
		{
			return instance._ImportModel(filePath, addToDatabase);
		}

		Model* _ImportModel(const string& filePath, bool addToDatabase = true)
		{
			Assimp::Importer Importer;
			const aiScene* aiscene = Importer.ReadFile(filePath.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals);
			//ai_scene = Importer.GetOrphanedScene();
			return _ImportModel(filePath, aiscene, addToDatabase);
		}

		Model* _ImportModel(const string& filePath, const aiScene* aiscene, bool addToDatabase = true)
		{
			if (aiscene) {
				Model* model = new Model();
				model->sourceFile = filePath;
				model->name = Save::GetFileName(filePath);
				for (unsigned int i = 0; i < aiscene->mNumMeshes; i++)
				{
					Mesh m;
					const aiMesh* aimesh = aiscene->mMeshes[i];
					ConvertMesh(i, aimesh,m);
					model->meshes.push_back(m);

				}
				if(addToDatabase)
					models.push_back(model);
				return model;
			}
			else {
				Assimp::Importer Importer;
				printf("Error parsing '%s': '%s'\n", filePath.c_str(), Importer.GetErrorString());
				return NULL;
			}
		}

		void ConvertMesh(unsigned int Index, const aiMesh* paiMesh,Mesh& mesh)
		{
			std::vector<unsigned int> Indices;
			const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
			mesh.faces.resize(paiMesh->mNumFaces);
			mesh.vertices.resize(paiMesh->mNumVertices);
			for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
			{
				const aiVector3D* pPos = &(paiMesh->mVertices[i]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

				Vertex v(glm::vec3(pPos->x, pPos->y, pPos->z),
					glm::vec2(pTexCoord->x, pTexCoord->y),
					glm::vec3(pNormal->x, pNormal->y, pNormal->z), (int)i);
				mesh.vertices[i] = v;
			}

			for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
			{
				const aiFace& F = paiMesh->mFaces[i];
				assert(F.mNumIndices == 3);
				Indices.push_back(F.mIndices[0]);
				Indices.push_back(F.mIndices[1]);
				Indices.push_back(F.mIndices[2]);

				Face f = Face();
				f.points[0] = mesh.vertices[F.mIndices[0]];
				f.points[1] = mesh.vertices[F.mIndices[1]];
				f.points[2] = mesh.vertices[F.mIndices[2]];

				f.faceIndices.x = F.mIndices[0];
				f.faceIndices.y = F.mIndices[1];
				f.faceIndices.z = F.mIndices[2];

				f.index = i;
				f.ComputeNormal();
				f.ComputeWeightedNormals();
				mesh.faces[i] = f;
			}

			assert(Indices.size() > 0);

			mesh.indices = Indices;
			ConvertWeights(mesh, paiMesh);
			//mesh.CalculateVertexNormals();
			mesh.Buffer();
		}

		bool ConvertWeights(Mesh& mesh, const aiMesh* aimesh)
		{
			mesh.HasWeights = false;

			if (aimesh->mNumBones > 0)
				mesh.skeleton.Init(aimesh);

			for (int i = 0; i<aimesh->mNumBones; i++)
			{
				aiBone* aiBone = aimesh->mBones[i]; // i is the ID of the current bone

				for (int j = 0; j<aiBone->mNumWeights; j++)
				{
					aiVertexWeight weight = aiBone->mWeights[j];
					mesh.HasWeights = true;

					//HERE WE'LL ACTUALLY FILL THE ARRAYS, WITH BOTH INDICES AND WEIGHTS.
					for (int k = 0; k < 4; k++)
					{
						if (mesh.vertices[weight.mVertexId].weights[k] == 0.0f)
						{
							//(above) IF THE CURRENT BONE WEIGHT IS EQUAL TO 0,
							//THEN IT HASN'T BEEN FILLED YET WITH AN ACTUAL WEIGHT.
							mesh.vertices[weight.mVertexId].weights[k] = weight.mWeight;
							mesh.vertices[weight.mVertexId].boneIDs[k] = i;
							mesh.vertices[weight.mVertexId].hasWeights = true;
							break;
						}
					}
				}
			}
			return mesh.HasWeights;
		}

		static AnimationData* ImportAnimation(const string& animName, const string& Filename,bool addToDatabase = true)
		{
			return instance._ImportAnimation(animName, Filename,addToDatabase);
		}

		AnimationData* _ImportAnimation(const string& animName, const string& filePath,bool addToDatabase = true)
		{
			Assimp::Importer m_Importer;
			const aiScene* S = m_Importer.ReadFile(filePath.c_str(), NULL);
			return _ImportAnimation(animName, filePath, S, addToDatabase);
		}

		static AnimationData* ImportAnimation(const string& animName, const string& Filename, const aiScene* S)
		{
			return instance._ImportAnimation(animName, Filename,S);
		}

		AnimationData* _ImportAnimation(const string& animName, const string& filePath, const aiScene* S,bool addToDatabase = true)
		{
			if (S != NULL)
			{
				AnimationData* a = new AnimationData();
				a->Init(animName, filePath, S);
				if (addToDatabase)
				{
					std::unordered_map<string, AnimationData*>::iterator search = animations.find(animName);
					if (search == animations.end())
					{
						animations.insert(std::make_pair(a->name, a));
						animationFiles.insert(std::make_pair(a->name, filePath));
					}
					else {
						animations[animName] = a;
						animationFiles[animName] = filePath;
					}
				}
				return a;
			}
			else {
				Assimp::Importer m_Importer;
				cout << ">>>>>>ERROR!<<<<<< Model::Problem parsing " << filePath.c_str() << m_Importer.GetErrorString() << endl;
				return NULL;
			}
		}

		void LoadAnimation(const string& filePath,bool load = false)
		{
			AnimationData* a = new AnimationData();
			//a->Init(animName, Filename, S);
			//a->LoadAnim(filePath);
			a->name = Save::GetFileName(filePath);
			a->save.file = filePath;
			animations.insert(std::make_pair(a->name, a));
			animationFiles.insert(std::make_pair(a->name, filePath));
		}

		void AddAnimationFile(string filePath)
		{
			string s = Save::GetFileName(filePath);
			animationFiles.insert(std::make_pair(s, filePath));
		}

		static void Load()
		{
			//instance.LoadAnimationDatabaseFile();
			instance.FindAllFBXAssetFiles();
		}

		static AnimationData* GetAnimation(const string animationName)
		{
			if (instance.animations.size() == 0)
				return NULL;
			std::unordered_map<string, AnimationData*>::iterator search = instance.animations.find(animationName);
			if (search == instance.animations.end())
				return NULL;
			return instance.animations.at(animationName);
		}

		void FindAllFBXAssetFiles()
		{
			std::vector<string> files;
			string dir = "Assets\\Animations\\";

			cout << "Assets Database: Loading FBX files from path: " << dir<<endl;

			Save::GetAllFilesTypes(dir, files);

			if (files.size() == 0)
			{
				cout << "Asset Database: no FBX files found at path: " << dir << endl;
				return;
			}

			for (string s : files)
			{
				if (s == "")
					continue;

				AddAnimationFile(s);
				cout << "FBX File found: " << s << endl;
			}
		}

		void LoadAnimationDatabaseFile()
		{
			save.file = "Assets\\animationDatabase.glax";
			std::vector<string> allLines;
			save.GetAllLines(allLines);

			for (const string& s : allLines)
			{
				if (s == "")
					continue;
				LoadAnimation(s);
			}
		}

		void SaveModelList()
		{
			save.file = "Assets\\modelDatabase.glax";
			if (!save.Exists())
			{
				save.CreateTextFile();
				cout << "Model Database file created: " << save.file << endl;
			}
			std::ofstream appendStream;
			std::ifstream readStream;
			save.OpenForReadAppend(readStream, appendStream);
			for (Model* model : models)
			{
				//if (!Save::FileExists(model->save.file))
				//	model->SaveFile();
				save.AppendLine(readStream, appendStream, model->save.file);
			}
			appendStream.close();
			readStream.close();
		}

		void SaveAnimationList()
		{
			save.file = "Assets\\animationDatabase.glax";
			if (!save.Exists())
			{
				save.CreateTextFile();
				cout << "Animation Database file created: " << save.file << endl;
			}
			std::ofstream appendStream;
			std::ifstream readStream;
			save.OpenForReadAppend(readStream, appendStream);
			for (auto data : animations)
			{
				if(Save::FileExists(data.second->save.file))
				save.AppendLine(readStream, appendStream, data.second->save.file);
			}
			appendStream.close();
			readStream.close();
		}

		void CleanUp()
		{
			SaveAnimationList();
			SaveModelList();
			for (auto data : animations)
			{
				data.second->CleanUp();
				delete data.second;
			}
			for (Model* m : models)
				delete m;

			models.clear();
			animations.clear();
		}
	};
}
