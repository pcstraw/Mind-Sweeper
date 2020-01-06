#pragma once
#include "NativeHeader.h"
#include "Input.h"
#include "Viewport.h"
#include "Serializer.h"
#include "Timestep.h"

nativenamespace{

	class Camera
	{
	public:
		/*
		Camera()
		{
		}
		Camera(const Camera& cam)
		{}

		Camera(Camera&& arg) : save(std::move(arg.save)) { }
		*/
		float fieldOfView = 90.f;
		float nearPlane = 0.1f;
		float farPlane = 10000.f;
		int width = defaultWIDTH;
		int height = defaultHEIGHT;
		vec3 position = vec3(0.f);
		//vec3 forward = vec3(0.f, 0.f, 1.f);
		//vec3 right = vec3(1.f, 0.f, 0.f);
		vec3 up = vec3(0.f, 1.f, 0.f);
		mat4 viewMatrix;
		mat4 inverseViewMatrix;
		mat4 projectionMatrix;
		mat4 inverseProjectionMatrix;
		mat4 projectionViewMatrix;
		quat orientation;
		Save save;

		void Init()
		{
			UpdateProjectionMatrix();
			UpdateViewMatrix();
		}

		vec3 Forward()
		{
			//return glm::normalize(rotation);
			return orientation * vec3(0,0,1);
		}

		vec3 Right()
		{
			//return glm::normalize(-glm::cross(up, rotation));
			return orientation * vec3(1, 0, 0);
		}
		vec3 Up()
		{
			//return glm::normalize(-glm::cross(up, rotation));
			return orientation * vec3(0, 1, 0);
		}
		vec3 localUp = vec3(0.f, 1.f, 0.f);
		mat4 UpdateViewMatrix()
		{
			vec3 lf = orientation * vec3(0, 0, 1);
			vec3 lu = orientation * vec3(0, 1, 0);
			viewMatrix = glm::lookAt(position, position - lf, lu);
			//mat4 oldview = viewMatrix;
			//mat4 newview = glm::lookAt(position, position - lf, lu);
			//viewMatrix = glm::mix(oldview, newview,Timestep::delta);
			inverseViewMatrix = glm::inverse(viewMatrix);
			projectionViewMatrix = projectionMatrix * viewMatrix;
			//up = vec3(viewMatrix[1]);
			//forward = glm::normalize(rotation);
			//right = glm::normalize(-glm::cross(up, rotation));
			return viewMatrix;
		}

		void SetProjection(float FieldOfView, float NearPlane, float FarPlane)
		{
			fieldOfView = FieldOfView;
			nearPlane = NearPlane;
			farPlane = FarPlane;
			UpdateProjectionMatrix();
		}

		mat4 UpdateProjectionMatrix()
		{
			float aspectRation = (float)width / (float)height;
			projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRation, nearPlane, farPlane);
			inverseProjectionMatrix = glm::inverse(projectionMatrix);
			return projectionMatrix;
		}
		/*
		void GetFarClipPoints()
		{
			float aspect = ratio;
			float fov = fieldOfView;
			float nearZ = nearPlane;
			float farZ = farPlane;

			float farH = 2 * tanf(fov / 2.0f) * farPlane;
			float farW = farH * aspect;
			float farX = farW / 2.0f;
			float farY = farH / 2.0f;
		}
		*/

		//see GITHUB reference
		//https://github.com/opengl-tutorials/ogl/blob/master/misc05_picking/misc05_picking_custom.cpp
		void GetMouseRay(         // Camera parameters (ratio, field of view, near and far planes)
			vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
			vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
		) {
			double mouseX = Input::mouse.x;
			double mouseY = (double)height - Input::mouse.y;
			// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
			glm::vec4 lRayStart_NDC(
				((float)mouseX / (float)width - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
				((float)mouseY / (float)height - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
				-1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
				1.0f
			);
			glm::vec4 lRayEnd_NDC(
				((float)mouseX / (float)width - 0.5f) * 2.0f,
				((float)mouseY / (float)height - 0.5f) * 2.0f,
				0.0,
				1.0f
			);

			glm::mat4 M = glm::inverse(projectionMatrix * viewMatrix);
			glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world /= lRayStart_world.w;
			glm::vec4 lRayEnd_world = M * lRayEnd_NDC; lRayEnd_world /= lRayEnd_world.w;

			glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
			lRayDir_world = glm::normalize(lRayDir_world);

			out_origin = glm::vec3(lRayStart_world);
			out_direction = glm::normalize(lRayDir_world);
		}

		vec2 GetScreen(const vec4 point)
		{
			vec2 p = vec2(point / point.w);
			vec2 halfdim = vec2(WIDTH, HEIGHT)*0.5f;
			p.x = (halfdim.x)*p.x + (1.0 + halfdim.x);
			p.y = HEIGHT - ((halfdim.y*p.y) + (1.0 + halfdim.y));
			//p.x = (p.x / (WIDTH / 2.f))/WIDTH;
			//p.y = (p.y / (HEIGHT / 2.f))/HEIGHT;
			//p.x = (2.f*point.x + 1.f) / WIDTH - 1.f;
			//p.y = -((2.f*point.y + 1.f) / HEIGHT - 1.f);
			return p;
		}

		vec2 WorldToScreen(const vec3& pos)
		{
			return GetScreen(ConvertToClip(pos));
		}

		vec4 ConvertToClip(const vec3& pos)
		{
			vec4 p = projectionViewMatrix * vec4(pos, 1.0);
			return p;
		}

		void BindWindowViewport()
		{
			BindViewport(Viewport::window);
		}

		void BindViewport(Viewport* viewport)
		{
			std::function<void(int, int)> _f = std::bind(&Camera::OnResize, this, std::placeholders::_1, std::placeholders::_2);
			Viewport::window->callbacks.push_back(_f);
		}

		void OnResize(int w, int h)
		{
			width = w;
			height = h;
			if (width == 0)
				width = 1;
			if (height == 0)
				height = 1;
			UpdateProjectionMatrix();
		}

		void SetViewport()
		{
			glViewport(0, 0, width, height);
		}

		void SaveToFile(const std::string& filePath)
		{
			save.CreateTextFile(filePath);
			save.OpenAndAddLine("Camera Name:");
			std::ostringstream ss;
			glm::vec3 pos = position;
			save.AddVec3("Camera::position::", pos);
			save.AddQuat("Camera::rotation::", orientation);
		}

		void LoadFromFile(const std::string& filePath)
		{
			std::vector<std::string> lines = save.GetLines(filePath, "Camera::position");
			if (lines.size() == 0)
				return;

			glm::vec3 pos = Save::ExtractVec3("Camera::position::", lines);
			position = pos;
			if (lines.size() == 0)
			{
				cout << "Camera: Load Save File: >>>>ERROR!<<<< Failed to load position values" << endl;
				return;
			}

			std::vector<std::string> tlines = save.GetLines(filePath, "Camera::rotation");
			if (tlines.size() == 0)
			{
				cout << "Camera: Load Save File: >>>>ERROR!<<<< Failed to load rotation values" << endl;
				return;
			}
			orientation = Save::ExtractQuat("Camera::rotation::", tlines);
		}

		void DebugLoadSave()
		{
			LoadFromFile("EditorCamera.glax");
		}
	};
}


