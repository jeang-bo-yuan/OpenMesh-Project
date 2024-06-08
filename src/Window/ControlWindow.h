#pragma once

#include <Scene/MainScene.h>
#include <string>
#include <Mesh/TextureManager.h>

namespace CG
{
	class ControlWindow
	{
	public:
		ControlWindow();

		auto Initialize() -> bool;
		void Display();

	private:
		bool showDemoWindow;
		int selectedState;
		std::string texcoordFilePath;

		// each texture's file name
		std::string textureFileArray[TextureManager::MAX_IMG];
		// is texture loaded
		bool textureLoaded[TextureManager::MAX_IMG];

	private:
		MainScene* targetScene;

	public:
		void SetTargetScene(MainScene* scene)
		{
			targetScene = scene;
		}
	};
}
