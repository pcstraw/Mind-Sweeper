#pragma once
#include "SceneBase.h"
#include "Modal.h"
#include "ShaderFactory.h"
#include "GimbalRotation.h"
#include "FrameBufferIndex.h"
#include "ShaderForScreenLines.h"
#include "Editor.h"

nativenamespace{

	class TestMeshScene : public SceneBase{

	public:

		Shader gimbalRotShader;
		Shader meshShader;
		ShaderForScreenLines screenShader;
		GimbalRotation gimbal;
		FramebufferIndex indexFrameBuffer;
		Model cone;
		uvec4 indexid = uvec4(0);

		void OnLoad() override
		{
			camera.DebugLoadSave();

			screenShader.Compile("Screen Shader");

			meshShader = ShaderFactory::GetMeshShader();
			meshShader.Compile();

			gimbalRotShader = ShaderFactory::GetGimbalRotShader();
			gimbalRotShader.Compile("Gimbal Rot Shader");

			gimbal.Build();

			indexFrameBuffer.CreateIndexFramebuffer(WIDTH, HEIGHT);

			cone = *Assets::ImportModel("Assets\\cone.fbx");
		}

		void TestDirectionHit()
		{
			vec2 a = vec2(5, 20);
			vec2 b = vec2(1000, 0);
			vec2 p = vec2(0, 10);

			Editor::MouseCameraControl();
			ClearColourAndDepth(viewportColour);
			glEnableDepth(false);
			glDisable(GL_CULL_FACE);

		}

		void OnUpdate() override
		{
			glViewport(0, 0, camera.width, camera.height);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glEnableDepth(true);
			glPolygonMode(GL_FRONT, GL_FILL);
			
			gimbalRotShader.Select();
			gimbal.transform.GetWorldMatrix();
			indexid = uvec4(0);
			vec3 rayStart = vec3(0.f);
			vec3 rayDir = vec3(0.f);
			camera.GetMouseRay(rayStart, rayDir);
			gimbal.UpdateOffset(rayStart, rayDir,mouse.position);

			if (!Input::mouse.IsButtonDown(0))
			{
				gimbal.grabbed = false;
				gimbal.hovered = false;
				gimbal.hitID = 0;
				
				checkGLError("clear gimbal draw check");
				DrawIndex();
				if (indexid.x > 0)
				{
					gimbal.hovered = true;
					gimbal.hitID = indexid.y;
				}
			}
			if (gimbal.hovered||gimbal.grabbed)
			{
				//DrawIndex();
				if (Input::mouse.IsButtonDown(0))
					gimbal.grabbed = true;
			}

			ClearColourAndDepth(viewportColour);
			gimbalRotShader.SetUniform1i("hitID", gimbal.hitID);
			gimbalRotShader.SetUniform4f("selectionColour", vec4(1.f));
			Renderer::DrawMesh(gimbalRotShader, camera, gimbal.transform.worldMatrix, gimbal.model.meshes[0]);

			if (!gimbal.grabbed)
			{
				//camera.DebugMouseControl();
				Editor::MouseCameraControl();
			}
			else {

				lineShader.Select();
				vec3 dir = vec3(0.f);
				float fmax = 10000000.f;
				vec4 col = vec4(0.f);
				if (gimbal.hitID == 1)
				{
					dir = vec3(fmax, 0.f, 0.f);
					col = vec4(1.f, 0.f, 0.f, 1.f);
				}
				else if (gimbal.hitID == 2)
				{
					dir = vec3(0.f, fmax, 0.f);
					col = vec4(0.f, 1.f, 0.f, 1.f);
				}
				else if (gimbal.hitID == 3)
				{
					dir = vec3(0.f, 0.f, fmax);
					col = vec4(0.f, 0.f, 1.f, 1.f);
				}

				/*
				vec3 a = gimbal.transform.getPosition() + dir;
				vec3 b = gimbal.transform.getPosition() - dir;
				lineShader.DrawLine(a, b, col, camera.projectionViewMatrix);

				vec2 s1 = ScreenToNDC(gimbal.screenmousepos);
				vec2 s2 = ScreenToNDC(gimbal.screenmouseold);
				vec2 mousepos = mouse.getNDCpos();
				
				screenShader.Select();
				screenShader.DrawLine(vec4(s1,0,1), vec4(s2,0,1),COLOUR_PURPLE, COLOUR_YELLOW, camera.projectionViewMatrix);
				*/
				//screenShader.DrawLine(vec4(0, 0, 0, 1), vec4(mousepos, 0, 1), COLOUR_YELLOW, camera.projectionViewMatrix);
				//return;
				//screenShader.DrawLine(vec2(0), mousepos, COLOUR_YELLOW, camera.projectionViewMatrix);
				//cout << "mouse screen: " << glmGetString(gimbal.screenmousepos) << endl;
				//cout << "mouse screen old: " << glmGetString(gimbal.screenmouseold) << endl;

			}

			meshShader.Select();
			cone.meshes[0].transform.setRotation(gimbal.newq);
			cone.meshes[0].transform.GetWorldMatrix();
			Renderer::DrawMesh(meshShader, &camera, cone.meshes[0]);

			lineShader.Select();
			lineShader.DrawOrigin(5.f, camera.projectionViewMatrix);
			lineShader.DrawLine(vec3(0), vec3(10000.f, 0.f, 0.f), COLOUR_RED, camera.projectionViewMatrix);
			lineShader.DrawLine(vec3(0), vec3(0.f, 10000.f, 0.f), COLOUR_GREEN, camera.projectionViewMatrix);
			lineShader.DrawLine(vec3(0), vec3(0.f,0.f,10000.f), COLOUR_BLUE, camera.projectionViewMatrix);

			if (gimbal.hovered)
			{
				glEnableDepth(false);
				lineShader.DrawPoint(gimbal.hitPoint, COLOUR_YELLOW, 2.f, camera.projectionViewMatrix);
				lineShader.DrawPoint(gimbal.oldhit, COLOUR_RED, 2.f, camera.projectionViewMatrix);
			}
		}

		void DrawIndex()
		{
			indexFrameBuffer.Begin();
			indexFrameBuffer.Clear();
			gimbalRotShader.SetUniform4x4Array("moffset", 3, false, gimbal.moffset);
			gimbalRotShader.SetUniform4vArray("coffset", 3, gimbal.coffset);
			gimbalRotShader.SetUniform1i("id", gimbal.id);
			Renderer::DrawMesh(gimbalRotShader, camera, gimbal.transform.worldMatrix, gimbal.model.meshes[0]);
			indexFrameBuffer.End();

			indexid = indexFrameBuffer.ReadIDBuffer(mouse.x, mouse.y);
			vec4 wpos = indexFrameBuffer.ReadPositionBuffer(mouse.x, mouse.y);

			if (indexid.x > 0)
			{
				//gimbal.hovered = true;
				//gimbal.hitID = indexid.y;
				gimbal.clickPoint = vec3(wpos);
			}
		}
	};
}
