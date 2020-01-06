#pragma once
#include "NativeHeader.h"
#include "SceneBase.h"

nativenamespace{

	class GameBase {

	public:

		std::vector<SceneBase*> scenes;
		int sceneIndex = 0;

		SceneBase* AddScene(SceneBase* scene,bool load = false,bool makeCurrent = false)
		{
			scene->index = scenes.size();

			if (load)
				scene->OnLoad();

			if (makeCurrent)
				sceneIndex = scene->index;

			scenes.push_back(scene);
			return scene;
		}

		void SelectScene(int index)
		{
			if (index < 0)
				index = 0;
			if (index >= scenes.size())
				index = scenes.size() - 1;
			sceneIndex = index;
		}

		virtual void OnLoad() {}

		virtual void OnUpdate(){}

		virtual void OnRender() 
		{
			scenes[sceneIndex]->OnRender();
		}

		virtual void CleanUp()
		{
			for (SceneBase* s : scenes)
				s->OnCleanUp();
		}
	};

}
