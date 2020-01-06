#pragma once
#include "ShaderForLines.h"
#include "Camera.h"
#include "Keyboard.h"
#include "Input.h"
#include "GLWindow.h"
#include "ShaderForModels.h"
#include "ShaderForSkeleton.h"
#include "ShaderDeferedLighting.h"

nativenamespace{
	class Global
	{
	public:

		static void Init(GLWindow* window)
		{
			Global::glWindow = window;
			Viewport::window = &window->viewport;
			Global::lineShader.Compile("Line Shader");
			Global::gameCamera.Init();
			gameCamera.DebugLoadSave();
			Global::gameCamera.BindWindowViewport();
			modelShader.Build("Default Model Shader");
			skeletonShader.BuildAndCompile();
			deferedShader.BuildAndCompile(gameCamera.width, gameCamera.height);
		}

		static ShaderForLines lineShader;
		static ShaderForModels modelShader;
		static SkinnedMeshShader skeletonShader;
		static ShaderDeferedLighting deferedShader;
		static Camera gameCamera;
		static GLWindow* glWindow;
		
		~Global()
		{
		}

	private:

	};
}

