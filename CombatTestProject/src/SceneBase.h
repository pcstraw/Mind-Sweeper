#pragma once
#include "Global.h"
#include "Modal.h"
#include "Renderer.h"
#include "AssetDatabase.h"

nativenamespace{

	class SceneBase {

	public:

		SceneBase() : camera(Global::gameCamera) , keyboard(Input::keyboard) , mouse(Input::mouse) , lineShader(Global::lineShader)
		{
		}

		string name;
		int index;
		vec4 viewportColour = vec4(0.1f, 0.f, 0.1f, 1.f);
		Camera& camera;
		Keyboard& keyboard;
		Mouse& mouse;
		ShaderForLines& lineShader;

		std::vector<Model*> models;
		std::vector<Model*> skinnedModels;

		void virtual OnLoad() {}

		void virtual OnUpdate() {}

		Model* LoadModel(const string& filePath)
		{
			Model* m = Assets::ImportModel(filePath);
			models.push_back(m);
			return m;
		}

		Model* LoadAnimatedModel(const string& animName,const string& filePath)
		{
			Model* m = Assets::ImportModel(filePath);
			skinnedModels.push_back(m);
			return m;
		}

		void virtual OnRender() 
		{
			camera.SetViewport();
			glPolygonMode(GL_FRONT, GL_FILL);
			glEnableDepth(true);
			//glEnableTransparency(true);
			glEnable(GL_CULL_FACE);
			Global::deferedShader.Begin();
			Renderer::DrawModels(Global::modelShader, camera, models);
			Renderer::DrawSkinnedModels(Global::skeletonShader, &camera, skinnedModels);

			Global::deferedShader.End();
			ClearColourAndDepth(viewportColour);
			Global::deferedShader.DrawLighting(&camera);
		}

		void virtual OnCleanUp()
		{
			for (Model* m : models)
				m = NULL; //probably don't need this
			models.clear();

			for (Model* m : skinnedModels)
				m = NULL;
			skinnedModels.clear();
		}
	};
}