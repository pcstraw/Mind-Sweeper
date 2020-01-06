#pragma once
#include "NativeHeader.h"
#include "EditorControls.h"


nativenamespace{

	class Editor
	{
	public:

		EditorCamera editorCamera;
		static Editor instance;

		static void MouseCameraControl(Key togglekey = Key::Q, bool updateViewMatrix = true, Key savekey = Key::C)
		{
			instance.editorCamera.MouseControl(Global::gameCamera, togglekey, updateViewMatrix, savekey);
		}

		static void MouseCameraControl(Camera& cam, Key togglekey = Key::Q, bool updateViewMatrix = true, Key savekey = Key::C)
		{
			instance.editorCamera.MouseControl(cam, togglekey, updateViewMatrix, savekey);
		}
	};
}
