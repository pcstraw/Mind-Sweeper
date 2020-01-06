#pragma once
#include "NativeHeader.h"
#include "Vertex.h"
#include "GPUMesh.h"
#include "Transform.h"
#include "Mesh.h"
#include "Serializer.h"

namespace Native {

	class Model
	{
	public:
		Model()
		{
			colour = vec4(1.f);
		};

		~Model()
		{
		};

		vec4 colour;
		string name;
		string sourceFile;
		Transform transform;
		std::vector<Mesh> meshes;
		Save save;
		
		void Buffer()
		{
			for (int i = 0; i < meshes.size(); i++)
				meshes[i].Buffer();
		}

		void SaveFile()
		{
			string filePath = "Assets\\" + name + ".mesh";
			cout << "Creating Mesh Save File: " << filePath << endl;
			save.CreateTextFile(filePath);

			save.EnableOutStream();
			save.AddLine( name);
			save.AddLine( sourceFile);
			save.AddLine( "::Mesh Count::" + to_string(meshes.size()));
			save.CloseOutStream();
			for (int i = 0; i < meshes.size(); i++)
			{
				meshes[i].Save(i, name,sourceFile);
			}

			
			cout << "Finished saving mesh file" << endl;
		}

		void LoadFile(string modelName,int meshCount)
		{
			string filePath = "Assets\\" + modelName + ".mesh";
			cout << "Load Mesh Save File: " << filePath << endl;

			//meshes.clear();
			if(meshes.size() != meshCount)
				meshes.resize(meshCount);
			for (int i = 0; i < meshCount; i++)
			{
				meshes[i].LoadFile(modelName,i);
			}

			/*
			std::vector<string> alllines;
			save.file = filePath;
			save.GetAllLines(alllines);
			for (string s : alllines)
			{
				cout << s << endl;
			}*/
			//save.CreateTextFile(filePath);
			/*
			save.EnableOutStream();
			save.AddLine(name);
			save.AddLine(sourceFile);
			save.AddLine("::Mesh Count::" + to_string(meshes.size()));

			for (int i = 0; i < meshes.size(); i++)
			{
				meshes[i].Serialize(i, save);
			}

			save.CloseOutStream();
			cout << "Finished saving mesh file" << endl;
			*/
		}

		void CleanUp()
		{
		}
	};
}

/*

void ExportAnimationAndBones(int exportFormatIndex, bool printFormat = false)
{
string path = "C:\\Users\\Solo\\Desktop\\Combat Test\\CombatTestProject\\CombatTestProject\\Assets\\test anim.collada";

for (Mesh& m : meshes)
{
m.skeleton.ExportBonesToBindPose();
m.skeleton.animations[0].ExportAnimationScene();
}
int formatCount = aiGetExportFormatCount();
std::vector< const aiExportFormatDesc*> formats;
for (int i = 0; i < formatCount; i++)
{
const aiExportFormatDesc* format = aiGetExportFormatDescription(i);
formats.push_back(format);
cout << "Export format: " << i << " : " << format->id << " : " << format->description << "\n>>>>>>>>>>>>>>>>>>>>>\n" << endl;
//cout << format->description <<"\n"<< endl;
}

Assimp::Exporter e;
e.Export(m_pScene, formats[exportFormatIndex]->id, path, 0);
const char* error = e.GetErrorString();
cout << "Export Error: " << error << endl;
//mAiExporter->Export(scene, mExportFormatDesc->id, mFilePath);
}
*/
