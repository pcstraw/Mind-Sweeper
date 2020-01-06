#pragma once
#include "EditorGUI.h"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "AnimationPlayback.h"

nativenamespace{

	class GUIDopeSheet {


	public:

		GUIDopeSheet() {}

		GUIDopeSheet(Model* m,const string& n)
		{
			model = m;
			//currentAnimation = Assets::instance.animations["idle"];
			//currentAnimation->LoadAnim();
			//animationPlayer.SetData(currentAnimation);
		}

		float _windowWdith = 0;
		bool _useSlider = true;
		float _dragAmount = 0.1f;
		bool menuOpended = false;
		AnimationPlayback animationPlayer;
		AnimationData* currentAnimation;
		Model* model;
		string assetPath = "Assets\\";
		string importExt = ".fbx";
		string importName = "";

		bool Show()
		{
			ImGui::Begin("DopeSheet", NULL);
			menuOpended = !gui::IsWindowCollapsed();
			_windowWdith = ImGui::GetWindowWidth();
			SelectAnimations();

			if (animationPlayer.Animate(*model))
			{

				string channelCount = "Channel Count: " + to_string(currentAnimation->channels.size());
				gui::Text(channelCount.c_str());
				gui::SameLine();
				gui::Checkbox("Use Sliders", &_useSlider);
				if (_useSlider)
				{
					gui::SameLine();
					gui::InputFloat("Slide Amount", &_dragAmount);
				}

				gui::PushItemWidth(_windowWdith * 0.25);
				gui::InputText("Name", &currentAnimation->name);
				gui::PopItemWidth();
				gui::SameLine(); if (gui::Button("Save"))
				{
					cout << "Export Animation: " << currentAnimation->name << endl;
					currentAnimation->SaveAnim();
				}

				gui::PushItemWidth(_windowWdith * 0.25);
				gui::InputText("File Path", &importName);
				gui::PopItemWidth();
				gui::SameLine(); if (gui::Button("Import"))
				{
					string importFile = assetPath + importName + importExt;
					cout << "Import Animation: " << importFile << endl;
					if (Save::FileExists(importFile))
					{
						currentAnimation = Assets::ImportAnimation(importName, importFile, true);

						//model->animations.push_back(currentAnimation);
						//model->SelectAnimation(index);
						animationPlayer.SetData(currentAnimation);
					}
					else
						cout << "File does not exist: " << importFile << endl;
				}

				gui::NewLine();
				if (PushDouble("Current Time", animationPlayer.time, 0.25) && animationPlayer.pause)
					animationPlayer.Animate(*model);
				gui::Checkbox("Pause", &animationPlayer.pause);
				for (auto& c : currentAnimation->channels)
				{
					Channel* channel = &c.second;
					if (gui::TreeNode(channel->name.c_str()))
					{
						ShowChannelPosition(channel->positionKeys);
						ShowChannelRot(channel->rotationKeys);
						ShowChannelScale(channel->scaleKeys);
						gui::TreePop();
					}
				}
			}
			ImGui::End();
			return menuOpended;
		}

		void SelectAnimations()
		{
			int _windowSizeVisible = gui::GetWindowPos().x + gui::GetWindowContentRegionMax().x;
			int _lastButton = 0;
			ImGuiStyle _style = gui::GetStyle();
			for (auto& file : Assets::instance.animationFiles)
			{
				const char* filename = file.first.c_str();
				
				int _buttonSize = gui::CalcTextSize(filename).x + 2 * _style.FramePadding.x;
				int _nextButton = _lastButton + _style.ItemSpacing.x + _buttonSize;
				if (_nextButton < _windowSizeVisible)
					gui::SameLine();

				if (gui::Button(filename))
				{
					currentAnimation = Assets::instance.GetAnimation(filename);
					if (currentAnimation == NULL||!currentAnimation->loaded)
					{
						currentAnimation = Assets::ImportAnimation(filename, file.second);
					}
					animationPlayer.SetData(currentAnimation);
				}
				_lastButton = gui::GetItemRectMax().x;
			}gui::NewLine();
		}

		void SelectGlaxAnimations()
		{
			for (auto& anim : Assets::instance.animations)
			{
				AnimationData* ad = anim.second;
				if (gui::Button(ad->name.c_str()))
				{
					if (!ad->loaded)
						ad->LoadAnim();
					currentAnimation = ad;
					animationPlayer.SetData(ad);
				}
				gui::SameLine();
			}gui::NewLine();
		}
		

		bool PushDouble(const char* text, double& value, float width = 0.1)
		{
			gui::PushItemWidth(_windowWdith * width);
			bool _isusing = false;
			if (!_useSlider)
				_isusing = gui::InputDouble(text, &value, ImGuiDataType_Double);
			else
				_isusing = gui::DragScalar(text, ImGuiDataType_Double, &value, _dragAmount);
			gui::PopItemWidth();
			return _isusing;
		}

		bool PushFloat(const char* text,float& value,float width = 0.1)
		{
			gui::PushItemWidth(_windowWdith * width);
			bool _isusing = false;
			if (!_useSlider)
				_isusing = gui::InputFloat(text, &value);
			else
				_isusing= gui::DragFloat(text, &value, _dragAmount);
			gui::PopItemWidth();
			return _isusing;
		}

		void PushTime(double& time)
		{
			gui::PushItemWidth(_windowWdith * 0.1);
			gui::InputDouble("Time", &time);
			gui::PopItemWidth();
		}

		void PushVec3(const char* text,vec3& vec)
		{
			gui::PushItemWidth(_windowWdith * 0.4);
			if (!_useSlider)
				gui::InputFloat3(text, &vec[0]);
			else
				gui::DragFloat3(text, &vec[0], _dragAmount);
			gui::PopItemWidth();
		}

		void PushQuat(const char* text, quat& q)
		{
			gui::PushItemWidth(_windowWdith * 0.4);
			if (!_useSlider)
				gui::InputFloat4(text, &q[0]);
			else
				gui::DragFloat4(text, &q[0], _dragAmount);
			gui::PopItemWidth();
		}

		bool PushButton(const char* text)
		{
			gui::PushItemWidth(_windowWdith * 0.1);
			return gui::Button(text);
		}

		void DeleteKey()
		{
			cout << "Delete key not implemented" << endl;
		}

		bool _setallpos = false;
		bool _updateallpos = false;
		vec3 _allposvalue = vec3(0.f);

		bool _setallrot = false;
		bool _updateallrot = false;
		quat _allrotvalue = quat(1.f, 0.f, 0.f, 0.f);

		bool _setallscale = false;
		bool _updateallscale = false;
		vec3 _allscalevalue = vec3(1.f);
		

		void ShowChannelPosition(std::vector<PositionKey>& positionKeys)
		{
			string treeText = "Position Keys  Count: " + to_string(positionKeys.size());

			if (gui::TreeNode(treeText.c_str()))
			{
				gui::Checkbox("Set All", &_setallpos);
				if(_setallpos)
				{ 
					gui::SameLine();
					PushVec3("", _allposvalue);
					gui::SameLine();
					gui::Checkbox("Update All",&_updateallpos);
				}
				for (int i = 0; i < positionKeys.size(); i++)
				{
					PushTime(positionKeys[i].time);

					if (_updateallpos)
						positionKeys[i].position = _allposvalue;

					gui::SameLine(); PushVec3(to_string(i).c_str(), positionKeys[i].position);
					
					gui::PushID(i);
					//gui::SameLine(); if(PushButton("Zero"))
					//					positionKeys[i].position = vec3(0); gui::PopItemWidth();
				
					gui::SameLine(); if(PushButton("Revert"))
						positionKeys[i].position = vec3(0);	gui::PopItemWidth();
					
					gui::SameLine(); if (PushButton("Delete Key")) { DeleteKey();}; gui::PopItemWidth();

					gui::PopID();
					//gui::NewLine();
				}
				gui::TreePop();
			}
		}

		void ShowChannelScale(std::vector<ScaleKey>& scaleKeys)
		{
			string treeText = "Scale Keys  Count: " + to_string(scaleKeys.size());
			//bool _updateAll = false;
			//vec3 _allValue = vec3(0);

			if (gui::TreeNode(treeText.c_str()))
			{
				gui::Checkbox("Set All", &_setallscale);
				if (_setallscale)
				{
					gui::SameLine();
					PushVec3("", _allscalevalue);
					gui::SameLine();
					gui::Checkbox("Update All", &_updateallscale);
				}

				for (int i = 0; i < scaleKeys.size(); i++)
				{
					PushTime(scaleKeys[i].time);

					if (_updateallscale)
						scaleKeys[i].scale = _allscalevalue;

					gui::SameLine(); PushVec3(to_string(i).c_str(), scaleKeys[i].scale);

					gui::PushID(i);
					//gui::SameLine(); if (PushButton("Zero"))
					//	scaleKeys[i].scale = vec3(0); gui::PopItemWidth();

					gui::SameLine(); if (PushButton("Revert"))
						scaleKeys[i].scale = vec3(0);	gui::PopItemWidth();

					gui::SameLine(); if (PushButton("Delete Key")) { DeleteKey(); }; gui::PopItemWidth();
					gui::PopID();
					//gui::NewLine();
				}
				gui::TreePop();
			}
		}

		void ShowChannelRot(std::vector<RotationKey>& rotKeys)
		{
			string treeText = "Rotation Keys  Count: " + to_string(rotKeys.size());

			if (gui::TreeNode(treeText.c_str()))
			{
				gui::Checkbox("Set All", &_setallrot);
				if (_setallrot)
				{
					gui::SameLine();
					PushQuat("", _allrotvalue);
					gui::SameLine();
					gui::Checkbox("Update All", &_updateallrot);
				}

				for (int i = 0; i < rotKeys.size(); i++)
				{
					PushTime(rotKeys[i].time);

					if (_updateallrot)
						rotKeys[i].rotation = _allrotvalue;

					gui::SameLine(); PushQuat(to_string(i).c_str(), rotKeys[i].rotation);

					gui::PushID(i);
					//gui::SameLine(); if (PushButton("Zero"))
					//	rotKeys[i].rotation = quat(1.f, 0.f, 0.f, 0); gui::PopItemWidth();

					gui::SameLine(); if (PushButton("Revert"))
						rotKeys[i].rotation = quat(1.f,0.f,0.f,0);	gui::PopItemWidth();

					gui::SameLine(); if (PushButton("Delete Key")) { DeleteKey(); }; gui::PopItemWidth();
					gui::PopID();
					//gui::NewLine();
				}
				gui::TreePop();
			}
		}
	};
}
