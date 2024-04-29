
#include "MainScene.h"

namespace CG
{
	MainScene::MainScene()
	{
		camera = nullptr;
		mesh = nullptr;
	}

	MainScene::~MainScene()
	{
	}

	auto MainScene::Initialize() -> bool
	{
		return LoadScene();
	}

	void MainScene::Update(double dt)
	{

	}

	void MainScene::Render()
	{
		glClearColor(0.0, 0.0, 0.0, 1); //black screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mesh->Render(camera->GetProjectionMatrix(), camera->GetViewMatrix());
	}

	void MainScene::OnResize(int width, int height)
	{
		std::cout << "MainScene Resize: " << width << " " << height << std::endl;
	}

	void MainScene::OnKeyboard(int key, int action)
	{
		std::cout << "MainScene OnKeyboard: " << key << " " << action << std::endl;		
	}
	
	auto MainScene::LoadScene() -> bool
	{
		camera = new Camera();
		// Camera matrix
		camera->LookAt(
			glm::vec3(0, 0, 1),   // Camera position in World Space
			glm::vec3(0, 0, 0),   // and looks at the origin
			glm::vec3(0, 1, 0)    // Head is up (set to 0,1,0 to look upside-down)
		);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glEnable(GL_MULTISAMPLE);
		glDepthMask(GL_TRUE);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

		mesh = new MyMesh();
		mesh->LoadFromFile("./res/models/xyzrgb_dragon_100k.obj");
		
		return true;
	}
}