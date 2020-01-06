#pragma once
#include "NativeHeader.h"
#include "Camera.h"
#include "EditorGUI.h"
#include "Timestep.h"

nativenamespace{

	class EditorCamera
	{
	public:

		bool toggleControl = true;
		float moveSpeed = 350.f;
		float rotationSpeed = 2.2f;
		float interpolateMoveSpeed = 1.5f;
		float interpolateRotationSpeed = 24.f;
		double oldX = 0;
		double oldY = 0;

		ImVec2 auto_size;
		bool _guistarted;
		vec3 targetpos;
		float deltaX = 0;
		float deltaY = 0;

		void MouseControl(Camera& cam, Key togglekey = Key::Q,bool updateViewMatrix = true,Key savekey = Key::C)
		{
			if (gui::IsKeyPressedMap(ImGuiKey_Tab))
				toggleControl = !toggleControl;

			if (!toggleControl)
				return;
			
			DrawGUI(cam,updateViewMatrix);

			if (EditorGUI::IsDraggingGUI())
			{
				oldX = Input::mouse.x;
				oldY = Input::mouse.y;
				return;
			}

			if (Input::keyboard.IsPressed(savekey))
				cam.SaveToFile("EditorCamera.glax");

			vec3 oldpos = cam.position;
			vec3 newpos = cam.position;
			//targetpos = oldpos;
			quat oldort = cam.orientation;
			quat newort = cam.orientation;
			float _ms = moveSpeed;
			deltaX = float(oldX - Input::mouse.x);
			deltaY = float(oldY - Input::mouse.y);
			float _ty = 5.f;
			float _tx = 5.f;
			//bool resettranslation = true;

			if (Input::mouse.IsRightButtonDown())
			{
				float xRot = -rotationSpeed * 0.1f * Timestep::delta * deltaX;
				float yRot = -rotationSpeed * 0.1f * Timestep::delta * deltaY;

				// Y rotation.
				quat yrot = glm::angleAxis(yRot, cam.Right());
				// X rotation.
				vec3 u = vec3(0.f, 1.f, 0.f);
				quat xrot = glm::angleAxis(xRot, u);
				newort = xrot * yrot*cam.orientation;
			}

			if (Input::mouse.IsLeftButtonDown())
			{
				if (abs(deltaY) > _ty)
					newpos -= (glm::sign(deltaY)*10 + (deltaY*0.5f))*cam.Forward() * vec3(1,0,1) * _ms * Timestep::delta;
				if (abs(deltaX) > _tx)
					newpos -=  (glm::sign(deltaX)*10 + (deltaX*0.5f))*cam.Right() * vec3(1,0,1) * _ms * Timestep::delta;
				//resettranslation = false;
				//targetpos += newpos - oldpos;
			}

			if (Input::mouse.IsWheelDown())
			{
				if (abs(deltaY) > _ty)
					newpos -= deltaY * cam.Forward() * _ms * Timestep::delta;
				if (abs(deltaX) > _tx)
					newpos -= deltaX * cam.Right() * _ms * Timestep::delta;
				//resettranslation = false;
			}

			double wheel = Input::mouse.delta;
			if (abs(wheel) > 0.0)
			{
				float _vs = moveSpeed * 10.f;
				newpos.y += (float)Input::mouse.delta * _vs * Timestep::delta;
				//resettranslation = false;
			}
			
			cam.orientation = glm::mix(oldort, newort, glm::clamp(Timestep::delta * interpolateRotationSpeed, 0.f, 1.f));
			cam.position = glm::mix(oldpos, newpos, glm::clamp(Timestep::delta * interpolateMoveSpeed, 0.0f, 1.0f));
			cam.UpdateViewMatrix();
			//if (resettranslation)
			//	targetpos = vec3(0);
			//Interpolate(cam);
			oldX = Input::mouse.x;
			oldY = Input::mouse.y;
		}

		void Interpolate(Camera& cam)
		{
			cam.position = glm::mix(cam.position, cam.position + targetpos, glm::clamp(Timestep::delta * interpolateMoveSpeed, 0.0f, 1.0f));
			//(newpos - oldpos)*Timestep::delta*0.5f;
			cam.UpdateViewMatrix();
		}

		void DrawGUI(Camera& cam,bool updateViewMatrix = true)
		{
			string labelName = "Drone Cam On";
			gui::Begin(labelName.c_str(), false, ImGuiWindowFlags_AlwaysAutoResize);
			gui::SetWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
			gui::SetWindowCollapsed(true, ImGuiCond_Once);
			if (gui::IsWindowCollapsed())
				gui::Text(("    " + labelName).c_str());
			else {
				gui::Text("camera settings...");
				gui::DragFloat3("Position", &cam.position[0]);
				gui::DragFloat("Move Speed", &moveSpeed);
				//gui::SetWindowCollapsed(true, ImGuiCond_Always);
				gui::NewLine();
				if(updateViewMatrix)
					cam.UpdateViewMatrix();
			}
			gui::End();
		}

		/*
		void DebugFPEditorControl(Key savekey = Key::C)
		{
			bool _forward = false;
			bool _back = false;
			bool _right = false;
			bool _left = false;
			bool _look = false;
			bool _up = false;
			bool _down = false;

			vec3 oldpos = position;
			vec3 oldrot = rotation;
			vec3 newpos = position;
			vec3 newrot = rotation;

			if (Input::keyboard.IsPressed(savekey))
				SaveToFile("Game camera.glax");

			if (Input::keyboard.IsDown(Key::W))
				_forward = true;
			if (Input::keyboard.IsDown(Key::S))
				_back = true;
			if (Input::keyboard.IsDown(Key::D))
				_right = true;
			if (Input::keyboard.IsDown(Key::A))
				_left = true;
			if (Input::keyboard.IsDown(Key::E))
				_up = true;
			if (Input::keyboard.IsDown(Key::Q))
				_down = true;
			if (Input::keyboard.IsDown(Key::Z))
				_look = true;

			//double xpos, ypos;
			GLFWwindow * window = Global::glWindow->glfwWindow;
			//glfwGetCursorPos(window, &xpos, &ypos);
			if (_look||_forward||_back||_left||_right||_up||_down)
			{
				float xDelta = float(oldX - Input::mouse.x);
				float yDelta = float(oldY - Input::mouse.y);
				float xRot = rotationSpeed*Timestep::delta * xDelta;
				float yRot = rotationSpeed*Timestep::delta * yDelta;
				// X rotation.
				newrot = glm::rotateY(rotation, xRot);
				// Y rotation.
				vec3 newDirection = glm::rotate(newrot, yRot, Right());
				float a = glm::dot(newDirection, vec3(0.f, 1.f, 0.f));
				if (abs(a) < 0.99f)
					newrot = newDirection;
			}

			if (_forward)
				newpos += Forward() * moveSpeed * Timestep::delta;

			if (_back)
				newpos -= Forward() * moveSpeed * Timestep::delta;

			if (_right)
				newpos += Right() * moveSpeed * Timestep::delta;

			if (_left)
				newpos-= Right() * moveSpeed * Timestep::delta;

			if (_up)
				newpos += vec3(0.f,1.f,0.f) * moveSpeed * Timestep::delta;

			if (_down)
				newpos -= vec3(0.f,1.f,0.f) * moveSpeed * Timestep::delta;

			rotation = glm::mix(oldrot, newrot, glm::clamp(Timestep::delta * interpolateMoveSpeed, 0.0f, 1.0f));
			position = glm::mix(oldpos, newpos, glm::clamp(Timestep::delta * interpolateRotationSpeed, 0.0f, 1.0f));

			oldX = Input::mouse.x;
			oldY = Input::mouse.y;

			UpdateViewMatrix();
		}
		*/
	};

}
