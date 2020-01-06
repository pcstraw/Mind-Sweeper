#pragma once
#include "NativeHeader.h"
#include "Global.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Skeleton.h"
//const char* glsl_version = "#version 130";

nativenamespace{

	class guiVec3
	{
	public:

		guiVec3() {}
		~guiVec3() { vec = NULL; }

		guiVec3(vec3* v)
		{
			vec = v;
			name = "vec3";
		}

		guiVec3(vec3* v, const string& variableName)
		{
			vec = v;
			name = variableName;
		}

		string name;
		vec3* vec;
		ImVec2 windowSize;
		bool started = false;

		void Show()
		{
			bool opened = false;
			vec2 sp = Global::gameCamera.WorldToScreen(*vec);
			string _name = string("paddd" + name);
			if (started)
				gui::SetWindowSize(windowSize);
			gui::Begin(name.c_str(), &opened, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings);

			gui::SetWindowCollapsed(true, ImGuiCond_FirstUseEver);

			if (gui::IsWindowCollapsed())
			{
				gui::Text(_name.c_str());
				if (started)
					gui::SetWindowSize(windowSize);
				gui::SetWindowPos(ImVec2(sp.x, sp.y));
			}
			else
			{
				gui::DragFloat3("", &(*vec)[0]);
				gui::NewLine();
			}
			if (!started)
			{
				ImGuiWindow* guiWindow = gui::GetCurrentWindow();
				ImVec2 auto_size = gui::CalcWindowExpectedSize(guiWindow);
				windowSize = auto_size;
				started = true;
			}
			gui::End();
		}
	};

class guiSkeleton
{
public:

	guiSkeleton() {}

	guiSkeleton(Skeleton* s,const string& n)
	{
		_skeleton = s;
		name = n;
		//name = getName(v);

		boneNames.clear();
		boneNames.resize(_skeleton->bones.size());
		boneCount = _skeleton->bones.size();
		for (int i = 0; i < boneCount; i++)
		{
			boneNames[i] = _skeleton->bones[i]->name.c_str();
		}
	}

	std::vector<const char*> boneNames;
	int boneCount = 0;
	string name;
	Skeleton* _skeleton;
	Bone* selectedBone;
	ImVec2 windowSize;
	bool started = false;
	int selectedItem = -1;
	string selectionName = "";

	void Show(const vec3& worldPosition)
	{
		bool opened = false;
		vec2 sp = Global::gameCamera.WorldToScreen(worldPosition);
		string _name = string("paddd" + name);
		//if (started)
		//	gui::SetWindowSize(windowSize);
		gui::Begin(name.c_str(), &opened, 0);
		
		gui::SetWindowCollapsed(true, ImGuiCond_FirstUseEver);
		//cout << "Window hovered: " << hovered << endl;
		//cout << id << endl;
		if (gui::IsWindowCollapsed())
		{
			gui::Text(_name.c_str());
			//if (started)
			//	gui::SetWindowSize(windowSize);
			gui::SetWindowPos(ImVec2(sp.x, sp.y));
		}
		else
		{
			const char** _names = &boneNames[0];
			
			//gui::PushItemWidth(gui::GetWindowWidth()/3.f);
			gui::ListBox(selectionName.c_str(), &selectedItem,_names , boneCount,boneCount);
			
			if (selectedItem > -1)
			{
				selectedBone = _skeleton->bones[selectedItem];
				selectionName = selectedBone->name;
			}
			/*
			for (int i = 0; i < _skeleton->bones.size(); i++)
			{
				Bone* b = _skeleton->bones[i];
				//gui::SetHoveredID(i);
				gui::Text(b->name.c_str());
			//	gui::Text(b->name.c_str());
			}
			*/
		}
		if (!started)
		{
			/*
			ImGuiWindow* guiWindow = gui::GetCurrentWindow();
			ImVec2 auto_size = gui::CalcWindowExpectedSize(guiWindow);
			windowSize = auto_size;
			started = true;
			*/
		}
		gui::End();
	}
};

	class EditorGUI
	{
	public:

		static bool _gimbalDragging;

		static void Init()
		{
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();

			// Setup Platform/Renderer bindings
			ImGui_ImplGlfw_InitForOpenGL(Global::glWindow->glfwWindow, true);
			const char* glsl_version = "#version 130";
			ImGui_ImplOpenGL3_Init(glsl_version);
			// Our state


			//ImGuiIO& io = ImGui::GetIO();
			ImFont* font1 = io.Fonts->AddFontFromFileTTF("C:/Users/Solo/Desktop/game engine research/Test Examples/ImGUI Test App/ImGUITest/Fonts/Arena Condensed.ttf", 30.0f);
			if (font1 == nullptr)
				bool breakHere = true;
		}

		static void Begin()
		{
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		static void EndFrame()
		{
			// Rendering
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		static void CleanUp()
		{
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		static void UnLockCamera()
		{
			if(_gimbalDragging)
			_gimbalDragging = false;
		}

		static void LockCamera()
		{
			_gimbalDragging = true;
		}

		static bool IsDraggingGUI()
		{
			return gui::IsAnyWindowHovered() || gui::IsAnyItemActive()||gui::IsItemActive()||gui::IsAnyWindowFocused()||_gimbalDragging;
			//_gimbalDragging = false;
			//return gui::IsMouseDragging(0);
		}
	};
}
