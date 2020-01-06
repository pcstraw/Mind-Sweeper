#pragma once
#include "SceneBase.h"
#include "Modal.h"
#include "ShaderFactory.h"
#include "Renderer.h"
#include "Bone.h"
#include "ShaderForBones.h"
#include "Editor.h"

nativenamespace{

	class TestDrawBoneScene : public SceneBase
	{
	public:

		Model testMesh;
		Shader meshShader;
		ShaderForBones boneShader;
		Mesh boneMesh;
		vec3 tip;
		guiVec3 boneTip;

		void OnLoad() override
		{
			camera.DebugLoadSave();

			meshShader = ShaderFactory::GetMeshShader();
			meshShader.Compile("Mesh Shader");

			testMesh = *LoadModel("Assets\\cone.fbx");

			boneMesh.gpuData = Bone::BuildBoneMesh();

			boneShader.Compile("Bone Shader");
			tip = vec3(0, 0, 50);
			boneTip = guiVec3(&tip,getName(tip));
		}

		void OnUpdate() override
		{
			Editor::MouseCameraControl();
			ClearColourAndDepth(vec3(0.1f, 0.f, 0.1f));

			//meshShader.Select();
			boneTip.Show();

			glDisable(GL_CULL_FACE);
			boneShader.Select();
			boneShader.DrawBone(tip, vec3(0, 0, -10), COLOUR_GREY, camera.projectionViewMatrix);
			//Renderer::DrawMesh(meshShader, &camera, boneMesh);

			lineShader.Select();
			lineShader.DrawOrigin(10.f, camera.projectionViewMatrix);
		}
	};
}
