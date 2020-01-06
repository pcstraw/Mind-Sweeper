#pragma once
#include "SceneBase.h"
#include "ShaderForLines.h"
#include "GimbalTranslation.h"
#include "Editor.h"

nativenamespace{

	class TestScene : public SceneBase
	{
	public:

		Shader sdfShader;
		Shader widgetShader;
		Shader gimbalShader;
		GimbalTranslation gimbal;
		vec3 rayStart = vec3(5.f, 2.f, 0.2f);
		vec3 rayDir = vec3(-1.0, 0.0, 0.0);
		vec3 oldMouse = vec3(0.f);

		void OnLoad() override
		{
			Global::gameCamera.DebugLoadSave();

			sdfShader.vertexString = Shader::LoadFile("src\\shaders\\sdf\\fullScreenSDF.vert");
			sdfShader.fragmentString = Shader::LoadFile("src\\shaders\\sdf\\debugSDF.frag");
			sdfShader.Compile("SDF shader");

			widgetShader.vertexString = Shader::LoadFile("src\\shaders\\geometry\\line3d.vert");
			widgetShader.geometryString = Shader::LoadFile("src\\shaders\\geometry\\line3d.geom");
			widgetShader.fragmentString = Shader::LoadFile("src\\shaders\\geometry\\line3d.frag");
			widgetShader.Compile("Widget shader");

			gimbalShader.vertexString   = Shader::LoadFile("src\\shaders\\geometry\\gimbal.vert");
			gimbalShader.geometryString = Shader::LoadFile("src\\shaders\\geometry\\gimbal.geom");
			gimbalShader.fragmentString = Shader::LoadFile("src\\shaders\\geometry\\gimbal.frag");
			gimbalShader.Compile("Gimbal shader");

			gimbal.Build();
		}

		void OnUpdate() override
		{
			//TestSDF();
			TestGimbal();
		}

		void TestGimbal()
		{
			Camera& cam = Global::gameCamera;

			cam.GetMouseRay(rayStart, rayDir);

			gimbal.scale = 1.0f;
			gimbal.axisLength = 4.f;
			//gimbal.transform.setEulerXYZ(0.f, 0.f, 25.f);
			gimbal.transform.GetWorldMatrix();

			if (!Input::mouse.IsButtonDown(0) && !Input::mouse.IsButtonDown(1) && !Input::mouse.IsButtonDown(2))
			{
				float dist = 0;
				gimbal.TestGimbalIntersection(rayStart, rayDir, dist);
			}

			vec3 rayEnd = rayStart + rayDir * 10.f;

			vec3 bbmin = vec3(0);
			vec3 bbmax = vec3(0);

			if (gimbal.hovered)
			{
				if (Input::mouse.IsButtonDown(0))
					gimbal.grabbed = true;
				gimbal.ConstrainToPlane(vec3(0, 0, 1), rayStart, rayDir);
				gimbal.UpdateOffset(rayStart, rayDir);
				ClearDepthOnly();
				Global::lineShader.Select();
				Global::lineShader.DrawPoint(gimbal.hitPoint, COLOUR_PURPLE, 4.f, cam.projectionViewMatrix);
			}

			if (!Input::mouse.IsButtonDown(0))
				gimbal.grabbed = false;

			if (gimbal.grabbed)
			{
				gimbal.UpdateOffset(rayStart, rayDir);
				gimbal.ConstrainToPlane(vec3(0, 0, 1), rayStart, rayDir);
			}
			else
				Editor::MouseCameraControl();


			ClearColourAndDepth(vec3(0.1f, 0.f, 0.1f));
			gimbalShader.Select();
			gimbalShader.SetProjection(cam.projectionViewMatrix);
			gimbalShader.SetModel(gimbal.transform.worldMatrix);
			gimbalShader.SetColour(vec4(1.f, 1.f, 0.f, 1.f));
			gimbalShader.SetUniform1f("scale", gimbal.scale);
			gimbalShader.SetUniform1f("axisLength", gimbal.axisLength);
			gimbalShader.SetUniform4f("selectionColour", vec4(1.f, 1.f, 0.f, 1.f));
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);
			gimbal.gpuMesh.DrawMesh();

			Global::lineShader.Select();
			Global::lineShader.DrawLine(rayStart, rayEnd, COLOUR_WHITE, COLOUR_RED, cam.projectionViewMatrix);

			Global::lineShader.DrawOrigin(5.f, Global::gameCamera.projectionViewMatrix);
		}

		void TestWidget()
		{
			Camera& cam = Global::gameCamera;
			glEnableDepth(true);
			
			widgetShader.Select();
			widgetShader.SetUniform3f("point", vec3(0));
			widgetShader.SetProjection(cam.projectionViewMatrix);
			widgetShader.SetModel(mat4(1.f));
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT, GL_FILL);
			glDrawArrays(GL_POINTS, 0, 1);

			ClearDepthOnly();
			Global::lineShader.Select();
			Global::lineShader.DrawOrigin(5.f, Global::gameCamera.projectionViewMatrix);
		}

		void TestSDF()
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			Camera& cam = Global::gameCamera;
			vec3 pos = cam.position;
			//cam.transform.setPosition(pos + vec3(0.0, 0.0, 1.0));
			//cam.target += vec3(0.0,0.0,1.0);
			Editor::MouseCameraControl();
			glm::mat4 eye_world = cam.inverseViewMatrix;
			glm::vec3 cam_eye = cam.position;
			glm::mat4 inverseView = cam.viewMatrix;
			//get camera axis
			//https://www.3dgep.com/understanding-the-view-matrix/#Convention
			glm::vec3 cam_right = glm::vec3(eye_world[0][0], eye_world[0][1], eye_world[0][2]);
			glm::vec3 cam_up = glm::vec3(eye_world[1][0], eye_world[1][1], eye_world[1][2]);
			glm::vec3 cam_forward = glm::vec3(eye_world[2][0], eye_world[2][1], eye_world[2][2]);
			glViewport(0, 0, cam.width, cam.height);
			sdfShader.Select();
			sdfShader.SetUniform3f("camforward", cam_forward);
			sdfShader.SetUniform3f("camup", cam_up);
			sdfShader.SetUniform3f("camright", cam_right);
			sdfShader.SetUniform3f("eyepos", cam_eye.x, cam_eye.y, cam_eye.z);
			sdfShader.SetUniform2f("screensize", ivec2(cam.width, cam.height));
			sdfShader.SetUniform1f("farplane", cam.farPlane);

			mat4 debugLight;
			debugLight[0] = vec4(pos + vec3(0.f,100.f,0.f), 0.05f);
			debugLight[1] = COLOUR_WHITE;
			sdfShader.SetUniform4x4("debuglight", debugLight);
			sdfShader.SetUniform4x4("projection", cam.projectionMatrix);
			sdfShader.SetUniform4x4("projection", cam.projectionMatrix);
			sdfShader.SetUniform4x4("inverseprojection", cam.inverseProjectionMatrix);
			sdfShader.SetUniform4x4("view", cam.viewMatrix);
			sdfShader.SetUniform4x4("perspective", cam.projectionViewMatrix);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			ClearDepthOnly();
			Global::lineShader.Select();
			Global::lineShader.DrawOrigin(5.f, Global::gameCamera.projectionViewMatrix);
		}
	};

}
