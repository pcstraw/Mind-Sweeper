#pragma once
#include "NativeHeader.h"
#include "SceneBase.h"
#include "Editor.h"
#include "PlayerControls.h"
#include "AnimationTrigger.h"
#include "AssetDatabase.h"
#include "Gimbal.h"

nativenamespace{

	class Test3rdPersonScene : public SceneBase
	{
	public:

		Model* model1;
		PlayerControls playerControls;
		AnimationTrigger animationController;
		GUIDopeSheet guiDopeSheet;
		Gimbal gimbal;
		
		void OnLoad() override
		{
			//model1 = LoadAnimatedModel("Assets\\test anim.collada");
			//model1 = LoadAnimatedModel("westerner","Assets\\models\\westerner f.fbx");
			model1 = LoadAnimatedModel("idle", "Assets\\Animations\\idle.fbx");
			model1->meshes[0].colour = COLOUR_RED;
			guiDopeSheet = GUIDopeSheet(model1, "Dope Sheet");

			camera.UpdateViewMatrix();

			gimbal.Build(5.f, 5.f);

			//model1->SaveFile();
			//model1->LoadFile("idle",2);
		}

		void OnUpdate() override
		{
			Editor::MouseCameraControl();

			if (!guiDopeSheet.Show())
			{
				playerControls.KeyboardControls(model1->transform, 50.f);
				animationController.WalkAnimation(model1->transform, *model1);
			}

			
			Global::deferedShader.testLight = gimbal.translationGimbal.transform.getPosition();
		}

		void OnRender() override
		{
			SceneBase::OnRender();

			lineShader.Select();
			lineShader.DrawOrigin(10.f, camera.projectionViewMatrix);

			gimbal.UseTranslationGimbal();
		}

		void OnCleanUp() override
		{
			model1->CleanUp();

			SceneBase::OnCleanUp();
		}
	};
}

