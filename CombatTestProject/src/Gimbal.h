#pragma once
#include "GimbalTranslation.h"
#include "GimbalRotation.h"
#include "Global.h"
#include "FrameBufferIndex.h"
#include "EditorGUI.h"

nativenamespace{

	class Gimbal
	{
	public:

		Gimbal() : camera(Global::gameCamera) , lineShader(Global::lineShader),keyboard(Input::keyboard) , mouse(Input::mouse)
		{}

		FramebufferIndex indexFrameBuffer;
		Shader gimbalShader; //translation
		Shader gimbalRotShader;
		Shader indexBufferBlit;
		GimbalTranslation translationGimbal;
		GimbalRotation rotationGimbal;
		ShaderForLines& lineShader;
		Camera& camera;
		Keyboard& keyboard;
		Mouse& mouse;
		uvec4 indexid = uvec4(0); //rot

		void Build(float translationGimbalScale = 5.f,float translationGimbalLength = 5.f,float rotationGimbalScale = 1.f)
		{
			translationGimbal.Build(translationGimbalScale,translationGimbalLength);
			rotationGimbal.Build(rotationGimbalScale);

			indexFrameBuffer.CreateIndexFramebuffer(WIDTH, HEIGHT);

			indexBufferBlit.vertexString = Shader::LoadFile("shaders\\Lighting\\deferedLighting.vert");
			indexBufferBlit.fragmentString = Shader::LoadFile("shaders\\geometry\\indexBufferBlit.frag");
			indexBufferBlit.Compile("Index Buffer Blit");

			gimbalShader.vertexString = Shader::LoadFile("shaders\\geometry\\gimbal.vert");
			gimbalShader.geometryString = Shader::LoadFile("shaders\\geometry\\gimbal.geom");
			gimbalShader.fragmentString = Shader::LoadFile("shaders\\geometry\\gimbal.frag");
			gimbalShader.Compile("Gimbal shader");

			gimbalRotShader = ShaderFactory::GetGimbalRotShader();
			gimbalRotShader.Compile("Gimbal Rot Shader");
		}

		bool IsUsing()
		{
			if (rotationGimbal.grabbed || translationGimbal.grabbed)
				return true;
			return false;
		}

		void UseTranslationGimbal()
		{
			vec3 rayStart = vec3(0.f);
			vec3 rayDir = vec3(0.f);
			camera.GetMouseRay(rayStart, rayDir);
			glViewport(0, 0, camera.width, camera.height);
			glEnableDepth(true);

			translationGimbal.transform.GetWorldMatrix();
			uvec4 indexid = uvec4(0);
			if (!Input::mouse.IsButtonDown(0))
			{
				translationGimbal.hovered = false;
				translationGimbal.hitID = 0;
				indexFrameBuffer.Clear();
				checkGLError("clear gimbal draw check");
				indexFrameBuffer.Begin();
				DrawTranslationGimbal();
				indexFrameBuffer.End();
				indexid = indexFrameBuffer.ReadIDBuffer(mouse.x, mouse.y);
				vec4 wpos = indexFrameBuffer.ReadPositionBuffer(mouse.x, mouse.y);

				if (indexid.x > 0)
				{
					translationGimbal.hovered = true;
					translationGimbal.hitID = indexid.y;
					translationGimbal.whitpoint = vec3(wpos);
					translationGimbal.UpdateOffset(rayStart, rayDir);
					//cout << ">>>> Gimbal ID: " << indexid.x << " >>>> axis id: " << indexid.y << " >>>> position: X: " << wpos.x << " Y: " << wpos.y << " Z: " << wpos.z << endl;
				}
			}
			if (translationGimbal.hovered)
			{
				translationGimbal.UpdateOffset(rayStart, rayDir);

				if (Input::mouse.IsButtonDown(0))
					translationGimbal.grabbed = true;
			}

			if (!Input::mouse.IsButtonDown(0))
				translationGimbal.grabbed = false;

			if (!translationGimbal.grabbed)
			{
				//Editor::MouseCameraControl();
				EditorGUI::UnLockCamera();
			}
			else {
				EditorGUI::LockCamera();
				translationGimbal.UpdateOffset(rayStart, rayDir);

				lineShader.Select();
				vec3 dir = vec3(0.f);
				float fmax = 10000000.f;
				vec4 col = vec4(0.f);
				if (translationGimbal.hitID == 1)
				{
					dir = vec3(fmax, 0.f, 0.f);
					col = vec4(1.f, 0.f, 0.f, 1.f);
				}
				else if (translationGimbal.hitID == 2)
				{
					dir = vec3(0.f, fmax, 0.f);
					col = vec4(0.f, 1.f, 0.f, 1.f);
				}
				else if (translationGimbal.hitID == 3)
				{
					dir = vec3(0.f, 0.f, fmax);
					col = vec4(0.f, 0.f, 1.f, 1.f);
				}

				vec3 a = translationGimbal.transform.getPosition() + dir;
				vec3 b = translationGimbal.transform.getPosition() - dir;
				lineShader.DrawLine(a, b, col, camera.projectionViewMatrix);
			}

			DrawTranslationGimbal();

			if (translationGimbal.hovered)
			{
				lineShader.Select();
				glEnableDepth(false);
				lineShader.DrawPoint(translationGimbal.hitPoint, COLOUR_YELLOW, 2.f, camera.projectionViewMatrix);
			}

			checkGLError("Post gimbal draw check");
		}

		void DrawTranslationGimbal()
		{
			gimbalShader.Select();
			gimbalShader.SetProjection(camera.projectionViewMatrix);
			gimbalShader.SetModel(translationGimbal.transform.worldMatrix);
			gimbalShader.SetColour(vec4(1.f, 1.f, 0.f, 1.f));
			gimbalShader.SetUniform1i("id", 1);
			gimbalShader.SetUniform1f("scale", translationGimbal.scale);
			gimbalShader.SetUniform1f("axisLength", translationGimbal.axisLength);
			gimbalShader.SetUniform1i("hitID", translationGimbal.hitID);
			gimbalShader.SetUniform4f("selectionColour", vec4(1.f, 1.f, 1.f, 1.f));
			//glEnable(GL_CULL_FACE);
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			translationGimbal.gpuMesh.DrawMesh();
		}

		void UseRotationGimbal()
		{
			gimbalRotShader.Select();

			rotationGimbal.transform.setPosition(rotationGimbal.position);
			rotationGimbal.transform.setScale(vec3(rotationGimbal.scale));

			rotationGimbal.transform.GetWorldMatrix();
			indexid = uvec4(0);
			vec3 rayStart = vec3(0.f);
			vec3 rayDir = vec3(0.f);
			camera.GetMouseRay(rayStart, rayDir);
			rotationGimbal.UpdateOffset(rayStart, rayDir, mouse.position);

			if (!Input::mouse.IsButtonDown(0))
			{
				rotationGimbal.grabbed = false;
				rotationGimbal.hovered = false;
				rotationGimbal.hitID = 0;

				checkGLError("clear gimbal draw check");
				DrawRotationIndex();
				if (indexid.x > 0)
				{
					rotationGimbal.hovered = true;
					rotationGimbal.hitID = indexid.y;
				}
			}
			if (rotationGimbal.hovered || rotationGimbal.grabbed)
			{
				//DrawRotationIndex();
				if (Input::mouse.IsButtonDown(0))
					rotationGimbal.grabbed = true;
			}

			//ClearColourAndDepth(viewportColour);
			//display the gimbal
			gimbalRotShader.Select();
			gimbalRotShader.SetUniform1i("hitID", rotationGimbal.hitID);
			gimbalRotShader.SetUniform4f("selectionColour", vec4(1.f));
			Renderer::DrawMesh(gimbalRotShader, camera, rotationGimbal.transform.worldMatrix, rotationGimbal.model.meshes[0]);

			if (!rotationGimbal.grabbed)
			{
			}
			else {
				EditorGUI::LockCamera();

				glEnableDepth(false);
				lineShader.Select();
				vec3 p = rotationGimbal.transform.getPosition();
				if (rotationGimbal.hitID == 1)
				{
					lineShader.DrawLine(p, p + rotationGimbal.localAxisConstraint*200.f*rotationGimbal.scale, COLOUR_RED, camera.projectionViewMatrix);
				}
				else if (rotationGimbal.hitID == 2)
				{
					lineShader.DrawLine(p, p + rotationGimbal.localAxisConstraint*200.f*rotationGimbal.scale, COLOUR_GREEN, camera.projectionViewMatrix);
				}
				else if (rotationGimbal.hitID == 3)
				{
					lineShader.DrawLine(p, p + rotationGimbal.localAxisConstraint*200.f*rotationGimbal.scale, COLOUR_BLUE, camera.projectionViewMatrix);
				}
			}

			if (rotationGimbal.hovered)
			{
				lineShader.Select();
				glEnableDepth(false);
				lineShader.DrawPoint(rotationGimbal.hitPoint, COLOUR_YELLOW, 4.f*rotationGimbal.scale, camera.projectionViewMatrix);
				lineShader.DrawLine(rotationGimbal.transform.getPosition(), rotationGimbal.hitPoint, COLOUR_YELLOW, camera.projectionViewMatrix);
				lineShader.DrawPoint(rotationGimbal.oldhit, COLOUR_BLACK, 4.f*rotationGimbal.scale, camera.projectionViewMatrix);
				lineShader.DrawLine(rotationGimbal.transform.getPosition(), rotationGimbal.oldhit, COLOUR_WHITE, camera.projectionViewMatrix);
			}
		}

		void DrawRotationIndex()
		{
			indexFrameBuffer.Begin();
			indexFrameBuffer.Clear();
			gimbalRotShader.SetUniform4x4Array("moffset", 3, false, rotationGimbal.moffset);
			gimbalRotShader.SetUniform4vArray("coffset", 3, rotationGimbal.coffset);
			gimbalRotShader.SetUniform1i("id", rotationGimbal.id);
			Renderer::DrawMesh(gimbalRotShader, camera, rotationGimbal.transform.worldMatrix, rotationGimbal.model.meshes[0]);
			indexFrameBuffer.End();

			indexid = indexFrameBuffer.ReadIDBuffer(mouse.x, mouse.y);
			vec4 wpos = indexFrameBuffer.ReadPositionBuffer(mouse.x, mouse.y);

			if (indexid.x > 0)
			{
				//gimbal.hovered = true;
				//gimbal.hitID = indexid.y;
				rotationGimbal.clickPoint = vec3(wpos);
			}
		}
	};
}
