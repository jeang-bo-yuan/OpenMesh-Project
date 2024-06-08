#pragma once

#include <Scene/MainScene.h>
#include <string>

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

	private:
		MainScene* targetScene;

	public:
		void SetTargetScene(MainScene* scene)
		{
			targetScene = scene;
		}
	};
}
