#pragma once
#include "GameBase.h"
#include "TestScene.h"
#include "TestAnimationScene.h"
#include "TestTexture.h"
#include "TestMeshScene.h"
#include "TestBopeSheetRender.h"
#include "TestDrawBoneScene.h"
#include "TestImGUI.h"
#include "TestCameraScene.h"
#include "Test3rdPersonScene.h"

nativenamespace{

	class TestCombatGame : public GameBase
	{
	public:

		TestScene testScene;
		TestAnimationScene testAnimationScene;
		TestTexture testTexture;
		TestMeshScene testMeshScene;
		TestBopeSheetRender testDopeSheet;
		TestDrawBoneScene testBoneScene;
		TestImGUIScene testImGUI;
		TestCameraScene testCameraScene;
		Test3rdPersonScene test3rdPersonScene;


		void OnLoad() override
		{
			//AddScene(&testScene, true, true);
		//	AddScene(&testAnimationScene, true, true);
		//	AddScene(&testTexture, true, true);
		//	AddScene(&testBoneScene, true, true);
			//AddScene(&testImGUI,true,true);
			//AddScene(&testMeshScene, true, true);
			//AddScene(&testDopeSheet, true, true);
			//AddScene(&testCameraScene, true, true);
			AddScene(&test3rdPersonScene, true, true);
		}

		void OnUpdate() override
		{
			if (scenes.size() == 0)
				return;

			scenes[sceneIndex]->OnUpdate();
		}
	};
}
