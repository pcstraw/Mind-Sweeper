#pragma once
#include "NativeHeader.h"
#include "SceneBase.h"
#include "Camera.h"
#include "Editor.h"

nativenamespace{


	class TestCameraScene : public SceneBase
	{
	public:

		Camera cam;

		void OnLoad() override
		{
			cam.position = vec3(0, 0, -10);
			cam.DebugLoadSave();
			cam.Init();
			cam.BindWindowViewport();
		}

		void OnUpdate() override
		{
			Editor::MouseCameraControl(cam);

			cam.SetViewport();
			ClearColourAndDepth(COLOUR_BLACK);

			lineShader.Select();
			lineShader.DrawOrigin(10.f, cam.projectionViewMatrix);
		}
	};
}

