#pragma once
#include "NativeHeader.h"
#include "GLWindow.h"
#include "Timestep.h"
#include "GameBase.h"
#include "Global.h"
#include "GLResource.h"
#include "GUIEngine.h"
#include "AssetDatabase.h"

nativenamespace{

	class Engine {

	public:

		Engine(GameBase* game)
		{
			gameBase = game;
		}

		float lastFrameTime = 0.f;
		bool _isResizing = false;
		GLWindow glWindow;
		GameBase* gameBase;

		void Run()
		{
			glWindow.Open();
			Global::Init(&glWindow);
			EditorGUI::Init();
			BindWindowViewport();
			Assets::Load();
			gameBase->OnLoad();

			while (!glfwWindowShouldClose(glWindow.glfwWindow))
			{
				UpdateGame();
			}
			gameBase->CleanUp();
			Assets::instance.CleanUp();
			EditorGUI::CleanUp();
			GLResource::ClearResources();
			glWindow.Close();
		}

		void UpdateGame()
		{
			float time = (float)glfwGetTime();
			Timestep::delta = time - lastFrameTime;
			lastFrameTime = time;
			glfwPollEvents();
			EditorGUI::Begin();
			gameBase->OnUpdate();
			gameBase->OnRender();
			Input::mouse.ResetInput();
			guiEngine::Show();
			EditorGUI::EndFrame();
			glfwSwapBuffers(glWindow.glfwWindow);
			
		}

		void BindWindowViewport()
		{
			std::function<void(int, int)> _f = std::bind(&Engine::OnResize, this, std::placeholders::_1, std::placeholders::_2);
			Viewport::window->callbacks.push_back(_f);
		}

		void OnResize(int w, int h)
		{
			UpdateGame();
		}
	};
}