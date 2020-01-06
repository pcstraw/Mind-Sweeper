#pragma once
#include "NativeHeader.h"
#include "SceneBase.h"
#include "Modal.h"
#include "ShaderForSkeleton.h"
#include "ShaderDeferedLighting.h"
#include "ShaderFactory.h"
#include "Renderer.h"
#include "Input.h"
#include "GimbalTranslation.h"
#include "FrameBufferIndex.h"
#include "Editor.h"

nativenamespace{

class TestAnimationScene : public SceneBase
{
public:

	Model model;
	SkinnedMeshShader skinnedShader;
	ShaderDeferedLighting deferedShader;
	vec3 lightpos = vec3(-2.0, 100.0, 50.0);
	GimbalTranslation gimbal;
	FramebufferIndex indexFrameBuffer;
	Shader gimbalShader;
	Shader indexBufferBlit;

	void OnLoad() override
	{
		name = "Test Animation Scene";

		camera = Global::gameCamera;
		camera.LoadFromFile("Game camera.glax");

		skinnedShader.BuildAndCompile();

		deferedShader.BuildAndCompile(camera.width, camera.height);
		deferedShader.testLight = lightpos;
		deferedShader.clearColour = viewportColour;

		LoadAnimatedModel();
		StartAnimation(true, false);

		viewportColour = vec4(0.05f, 0.f, 0.08f, 1.0f);

		indexFrameBuffer.CreateIndexFramebuffer(WIDTH, HEIGHT);

		indexBufferBlit.vertexString = Shader::LoadFile("src\\shaders\\Lighting\\deferedLighting.vert");
		indexBufferBlit.fragmentString = Shader::LoadFile("src\\shaders\\geometry\\indexBufferBlit.frag");
		indexBufferBlit.Compile("Index Buffer Blit");

		gimbalShader.vertexString = Shader::LoadFile("src\\shaders\\geometry\\gimbal.vert");
		gimbalShader.geometryString = Shader::LoadFile("src\\shaders\\geometry\\gimbal.geom");
		gimbalShader.fragmentString = Shader::LoadFile("src\\shaders\\geometry\\gimbal.frag");
		gimbalShader.Compile("Gimbal shader");

		//gimbal.Build(1.f,10.f);
		gimbal.Build(5.f, 5.f);
	}

	void OnUpdate() override
	{
		if (keyboard.IsPressed(Key::C))
			camera.SaveToFile("Game camera.glax");

		if (keyboard.IsPressed(Key::F))
			StartAnimation(true,false);

		if (keyboard.IsPressed(Key::R))
			StartAnimation(true, true);

		if (keyboard.IsPressed(Key::P))
			TogglePauseAnimation();

		if (keyboard.IsPressed(Key::Right))
			StepAnimation(false);

		if (keyboard.IsPressed(Key::Left))
			StepAnimation(true);

		deferedShader.testLight = gimbal.transform.getPosition();

		deferedShader.Begin();
		glViewport(0, 0, camera.width,camera.height);
		glEnableDepth(true);
		
		glEnable(GL_CULL_FACE);
		glEnableTransparency(false);
		ClearColourAndDepth(vec4(-1.f));
		skinnedShader.Select();
		if(model.meshes.size() > 0)
			Renderer::DrawSkeleton(skinnedShader,model, &camera );
		deferedShader.End();

		glEnableDepth(true);
		ClearColourAndDepth(viewportColour);
		glEnableTransparency(true);
		deferedShader.DrawLighting(&camera);
		
		checkGLError("Pre gimbal draw check");
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		ClearDepthOnly();
		//LineTest();
		UseGimbal();
		//camera.DebugMouseControl();
		//DrawGimbal();

		checkGLError("Post gimbal draw check");

		lineShader.Select();
		lineShader.DrawOrigin(10.f, mat4(1.f), camera.projectionViewMatrix);
	}

	void LineTest()
	{
		lineShader.Select();

		vec3 ro = vec3(0.f);
		vec3 rd = vec3(0.f);
		camera.GetMouseRay(ro, rd);

		vec3 L11 = vec3(1, 0, 0);

		vec3 L21 = vec3(-100, -1, 0);

		lineShader.DrawLine(L11, L11*100.f, COLOUR_RED, camera.projectionViewMatrix);
		lineShader.DrawLine(ro, ro+ rd*100.f, COLOUR_YELLOW, camera.projectionViewMatrix);
		vec3 i1 = vec3(0);
		vec3 i2 = vec3(0);
		bool intersect = gimbal.LineLineIntersection(vec3(0),L11, ro,(ro + rd*10.f), 0.1f, i1,i2);
		//lineShader.DrawPoint(i1, COLOUR_LIGHT_BLUE, 5.f, camera.projectionViewMatrix);
		//lineShader.DrawPoint(i2, COLOUR_RED, 5.f, camera.projectionViewMatrix);
		//cout << "List dist: " << dist << endl;
		if (intersect)
		{
			lineShader.DrawPoint(i1, COLOUR_LIGHT_BLUE, 5.f, camera.projectionViewMatrix);
			lineShader.DrawPoint(i2, COLOUR_LIGHT_BLUE, 5.f, camera.projectionViewMatrix);
		}
	}

	void UseGimbal()
	{
		vec3 rayStart = vec3(0.f);
		vec3 rayDir = vec3(0.f);
		camera.GetMouseRay(rayStart, rayDir);
		glViewport(0, 0, camera.width, camera.height);
		glEnableDepth(true);

		gimbal.transform.GetWorldMatrix();
		uvec4 indexid = uvec4(0);
		if (!Input::mouse.IsButtonDown(0))
		{
			gimbal.hovered = false;
			gimbal.hitID = 0;
			indexFrameBuffer.Clear();
			checkGLError("clear gimbal draw check");
			indexFrameBuffer.Begin();
			DrawGimbal();
			indexFrameBuffer.End();
			indexid = indexFrameBuffer.ReadIDBuffer(mouse.x, mouse.y);
			vec4 wpos = indexFrameBuffer.ReadPositionBuffer(mouse.x, mouse.y);

			if (indexid.x > 0)
			{
				gimbal.hovered = true;
				gimbal.hitID = indexid.y;
				gimbal.whitpoint = vec3(wpos);
				gimbal.UpdateOffset(rayStart,rayDir);
				//cout << ">>>> Gimbal ID: " << indexid.x << " >>>> axis id: " << indexid.y << " >>>> position: X: " << wpos.x << " Y: " << wpos.y << " Z: " << wpos.z << endl;
			}
		}
		if (gimbal.hovered)
		{
			gimbal.UpdateOffset(rayStart,rayDir);

			if (Input::mouse.IsButtonDown(0))
			{
				gimbal.grabbed = true;
			}
		}

		if (!Input::mouse.IsButtonDown(0))
			gimbal.grabbed = false;

		if (!gimbal.grabbed)
		{
			Editor::MouseCameraControl();
		}
		else {
			gimbal.UpdateOffset(rayStart, rayDir);

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

			vec3 a = gimbal.transform.getPosition() + dir;
			vec3 b = gimbal.transform.getPosition() - dir;
			lineShader.DrawLine(a, b, col, camera.projectionViewMatrix);
		}

		DrawGimbal();

		if (gimbal.hovered)
		{
			lineShader.Select();
			glEnableDepth(false);
			lineShader.DrawPoint(gimbal.hitPoint, COLOUR_YELLOW, 2.f, camera.projectionViewMatrix);
		}

		checkGLError("Post gimbal draw check");
	}

	void DrawGimbal()
	{
		gimbalShader.Select();
		gimbalShader.SetProjection(camera.projectionViewMatrix);
		gimbalShader.SetModel(gimbal.transform.worldMatrix);
		gimbalShader.SetColour(vec4(1.f, 1.f, 0.f, 1.f));
		gimbalShader.SetUniform1i("id", 1);
		gimbalShader.SetUniform1f("scale", gimbal.scale);
		gimbalShader.SetUniform1f("axisLength", gimbal.axisLength);
		gimbalShader.SetUniform1i("hitID", gimbal.hitID);
		gimbalShader.SetUniform4f("selectionColour", vec4(1.f, 1.f, 1.f, 1.f));
		//glEnable(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		gimbal.gpuMesh.DrawMesh();
	}

	void LoadAnimatedModel()
	{
		model = *Assets::ImportModel("Assets\\running fix.fbx");
		model.meshes[0].colour = COLOUR_RED;
		model.meshes[1].colour = COLOUR_YELLOW;
		model.transform.setPosition(vec3(0));

		for (int i = 0; i < model.meshes.size(); i++)
		{
			model.meshes[i].transform.setScale(vec3(1.f));
		}
	}

	void StartAnimation(bool loop, bool reverse)
	{
		for (int i = 0; i < model.meshes.size(); i++)
		{
			/*
			model.animationPlayback.loop = loop;
			model.animationPlayback.reverse = reverse;
			model.animationPlayback.duration = 22.0;
			*/
		}
	}

	void TogglePauseAnimation()
	{
		/*
		for (int i = 0; i < model.meshes.size(); i++)
			model.animationPlayback.pause = !model.animationPlayback.pause;
			*/
	}

	void StepAnimation(bool reverse)
	{
		/*
		bool olddirection = model.animationPlayback.reverse;
		model.animationPlayback.reverse = reverse;
		model.animationPlayback.pause = false;
		model.animationPlayback.Animate();
		model.animationPlayback.pause = true;
		model.animationPlayback.reverse = olddirection;
		*/
	}
};
}

