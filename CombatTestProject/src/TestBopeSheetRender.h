#pragma once
#include "NativeHeader.h"
#include "SceneBase.h"
#include "Modal.h"
#include "ShaderForSkeleton.h"
#include "ShaderDeferedLighting.h"
#include "ShaderFactory.h"
#include "Renderer.h"
#include "Input.h"
#include "Gimbal.h"
#include "Editor.h"
#include "ShaderForBones.h"
#include "AnimationEditor.h"
#include "FrameBufferIndex.h"

nativenamespace{

class TestBopeSheetRender : public SceneBase
{
public:

	Model model;
	GPUMesh boneMesh;
	Shader meshShader;
	ShaderForBones boneShader;
	SkinnedMeshShader skinnedShader;
	SkinnedMeshShader skinnedShaderInstanced;
	ShaderDeferedLighting deferedShader;
	FramebufferIndex indexBuffer;
	vec3 lightpos = vec3(-2.0, 100.0, 50.0);
	Gimbal gimbal;
	int hoveredBoneID = -1;
	int selectedBoneID = -1;
	guiSkeleton _guiSkeleton;
	GUIDopeSheet guiDopeSheet;
	std::vector<mat4> offsets;
	std::vector<std::vector<mat4>> bonelist;
	std::vector<std::vector<mat4>> bonelist2;
	
	void OnLoad() override
	{
		name = "Test Animation Scene";

		//camera = Global::gameCamera;
		//camera.DebugLoadSave();

		skinnedShader.BuildAndCompile();
		skinnedShaderInstanced.BuildAndCompileInstanced();

		deferedShader.BuildAndCompile(camera.width, camera.height);
		deferedShader.testLight = lightpos;
		deferedShader.clearColour = viewportColour;

		LoadAnimatedModel();
		StartAnimation(true, false);
		boneMesh = Bone::BuildBoneMesh();

		_guiSkeleton = guiSkeleton(&model.meshes[1].skeleton,"skeleton");

		meshShader = ShaderFactory::GetMeshShader();
		meshShader.Compile("Mesh Shader");

		boneShader.Compile("Bone Shader");

		viewportColour = vec4(0.05f, 0.f, 0.08f, 1.0f);

		gimbal.Build(1.f,10.f,0.05f);

		indexBuffer.CreateIndexFramebuffer(WIDTH, HEIGHT);

		guiDopeSheet = GUIDopeSheet(&model, "Dope Sheet");

		TogglePauseAnimation();
	}

	void OnUpdate() override
	{
		//if(!Editor::IsDraggingGUI())
		//	camera.DebugMouseControl();
		Editor::MouseCameraControl();
		

		if (keyboard.IsPressed(Key::F))
			StartAnimation(true,false);

		if (keyboard.IsPressed(Key::R))
			StartAnimation(true, true);

		if (keyboard.IsPressed(Key::P))
			TogglePauseAnimation();

		if (keyboard.IsPressed(Key::Right))
			StepAnimation(false);

		if (keyboard.IsPressed(Key::Left))
			StepAnimation(true);

		//draw animated model
		deferedShader.testLight = gimbal.translationGimbal.transform.getPosition();
		deferedShader.Begin();
		camera.SetViewport();
		glEnableDepth(true);
		glEnable(GL_CULL_FACE);
		glEnableTransparency(false);
		ClearColourAndDepth(vec4(-1.f));
		skinnedShader.Select();
		if (model.meshes.size() > 0)
			Renderer::DrawSkeleton(skinnedShader, model, &camera);
		
		deferedShader.End();

		glEnableDepth(true);
		ClearColourAndDepth(viewportColour);
		glEnableTransparency(true);
		deferedShader.DrawLighting(&camera);
		
		ClearDepthOnly();

		glEnable(GL_CULL_FACE);
		DrawSkeletonBones(model.meshes[1].skeleton, model.transform.worldMatrix, camera, COLOUR_GREY);

		ClearDepthOnly();
		/*
		for (int i = 0; i<model.meshes.size(); i++)
		{
			vec4 col = vec4(0, 0, 0, 1);
			col[i] = 1;
			DrawDebugSkeleton(model.meshes[i].skeleton, model.transform.worldMatrix, camera, col, col);
		}
		*/

		ClearDepthOnly();
		checkGLError("Post gimbal draw check");

		guiDopeSheet.Show();

		lineShader.Select();
		lineShader.DrawOrigin(10.f, mat4(1.f), camera.projectionViewMatrix);
	}

	void DrawSkeletonBones(Skeleton& skeleton, const mat4& transform, const Camera& cam, const vec4& colour)
	{
		std::vector<Bone*>& bones = skeleton.bones;
		indexBuffer.Clear();
		indexBuffer.Begin();
		//indexBuffer.SetDrawBuffer(3);
		indexBuffer.BlitFromDefault();
		boneShader.Select();
		
		for (int i = 0; i<bones.size(); i++)
		{
			Bone* b = bones[i];
			boneShader.SetUniform1i("boneID",i + 1);
			boneShader.DrawBone(b, cam.projectionViewMatrix, mat4(1.f), colour, 10.f);
		}
		indexBuffer.End();
		uvec4 ids = indexBuffer.ReadIDBuffer();
		//indexBuffer.SetDrawBuffer(1);
		indexBuffer.BlitToDefault();
		//uvec4 ids = indexBuffer.ReadIDBuffer(mouse.x, mouse.y);
		int hitID = ids.y - 1;
		if (hitID > -1 && !gimbal.IsUsing())
		{
			hoveredBoneID = hitID;
			//cout << "Bone ID: " << ids.y << " "<<bones[hitID]->name<< endl;
		}
		if (hitID == -1)
			hoveredBoneID = -1;

		if (hoveredBoneID > -1)
		{
			boneShader.DrawBone(bones[hoveredBoneID], cam.projectionViewMatrix, mat4(1.f), COLOUR_WHITE, 10.f);
			if (Input::mouse.IsButtonDown(0))
				selectedBoneID = hoveredBoneID;
		}

		if (selectedBoneID > -1)
		{
			//use model path instead.  or to where ever we store animations
			RotationKey* rk = guiDopeSheet.currentAnimation->GetRotationKey(bones[selectedBoneID]->name, guiDopeSheet.animationPlayer.time);
			if (rk != NULL)
			{
				gimbal.rotationGimbal.transform.setRotation(rk->rotation);
				gimbal.rotationGimbal.transform.GetWorldMatrix();
				gimbal.rotationGimbal.newq = rk->rotation;
				DrawGimbal(bones[selectedBoneID]->GetWorldPosition());
				rk->rotation = gimbal.rotationGimbal.newq;
			}
		}

		lineShader.Select();
		for (int i = 0; i<bones.size(); i++)
		{
			Bone* b = bones[i];
			DrawBonesAxis(b, 2.f);
		}
	}

	void DrawBonesAxis(const Bone* bone,float scale)
	{
		mat4 mat = bone->worldMatrix;
		lineShader.DrawAxis(vec3(0), scale, mat, camera.projectionViewMatrix);
	}

	void DrawDebugSkeleton(Skeleton& skeleton,const mat4& transform,const Camera& cam,const vec4& fromColour,const vec4& toColour)
	{
		lineShader.Select();
		std::vector<Bone*>& bones = skeleton.bones;
		for (int i = 0; i<bones.size(); i++)
		{
			Bone* b = bones[i];
			vec3 p = b->GetWorldPosition();
			Bone* parentBone = skeleton.GetParentBone(*guiDopeSheet.currentAnimation,b);
			//lineShader.DrawAxis(p, 1.f, transform, cam.projectionViewMatrix);
			if (parentBone == NULL)
				continue;
			vec3 parentPosition = parentBone->GetWorldPosition();
			lineShader.DrawLine(p, parentPosition, fromColour, toColour, cam.projectionViewMatrix);
			//Renderer::DrawMesh(meshShader, boneMesh, camera, m, vec4(1));
		}
	}

	void DrawGimbal(const vec3& pos)
	{
		gimbal.rotationGimbal.position = pos;
		gimbal.UseRotationGimbal();
	}

	void LoadAnimatedModel()
	{
		model = *LoadModel("Assets\\running fix.fbx");
		model.meshes[0].colour = COLOUR_RED;
		model.meshes[1].colour = COLOUR_YELLOW;
		model.transform.setPosition(vec3(0));

		for (int i = 0; i < model.meshes.size(); i++)
		{
			model.meshes[i].transform.setScale(vec3(1.f));
			//model.meshes[i].aiSkeleton.FindAndExcludeRoot("hips");
			//model.meshes[i].skeleton.FindAndExcludeRoot("hips");
		}
	}

	void StartAnimation(bool loop, bool reverse)
	{
		guiDopeSheet.animationPlayer.loop = loop;
		guiDopeSheet.animationPlayer.reverse = reverse;
	}

	void TogglePauseAnimation()
	{
		for (int i = 0; i < model.meshes.size(); i++)
			guiDopeSheet.animationPlayer.pause = !guiDopeSheet.animationPlayer.pause;
	}

	void StepAnimation(bool reverse)
	{
		bool olddirection = guiDopeSheet.animationPlayer.reverse;
		guiDopeSheet.animationPlayer.reverse = reverse;
		guiDopeSheet.animationPlayer.pause = false;
		guiDopeSheet.animationPlayer.Animate(model);
		guiDopeSheet.animationPlayer.pause = true;
		guiDopeSheet.animationPlayer.reverse = olddirection;
	}
};
}

