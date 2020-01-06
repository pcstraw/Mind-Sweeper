#pragma once
#include "NativeHeader.h"
#include "Timestep.h"
#include "EditorGUI.h"

nativenamespace{

	class guiEngine
	{
	public:

		

		static void Show()
		{
			gui::Begin("Engine", false);
			if (!gui::IsWindowCollapsed())
			{
				string deltaTime = "FPS: " + FloatToString(Timestep::delta);
				gui::Text(deltaTime.c_str());
			}
			gui::End();
		}
	};
}
