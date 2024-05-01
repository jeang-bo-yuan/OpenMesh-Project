
#include "MainScene.h"
#include <assert.h>

namespace CG
{
	MainScene::MainScene()
		: m_leftMouse(false), m_rightMouse(false), m_lastCursorPos()
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
		if (width == 0 || height == 0) return;
		camera->SetAspect((float)width / height);
	}

	void MainScene::OnKeyboard(int key, int action)
	{
		std::cout << "MainScene OnKeyboard: " << key << " " << action << std::endl;

		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			constexpr float move_speed = 0.01f;

			if (key == GLFW_KEY_W) {
				camera->changeCenter(move_speed * camera->GetFront());
			}
			else if (key == GLFW_KEY_S) {
				camera->changeCenter(-move_speed * camera->GetFront());
			}
			else if (key == GLFW_KEY_A) {
				camera->changeCenter(-move_speed * camera->GetRight());
			}
			else if (key == GLFW_KEY_D) {
				camera->changeCenter(move_speed * camera->GetRight());
			}
			else if (key == GLFW_KEY_Q) {
				camera->changeCenter(glm::vec3(0, move_speed, 0));
			}
			else if (key == GLFW_KEY_E) {
				camera->changeCenter(glm::vec3(0, -move_speed, 0));
			}
		}
	}

	void MainScene::OnCursorMove(float xpos, float ypos)
	{
		switch (m_current_state) {
		case State::RotateCamera:
			if (m_leftMouse) {
				float xoffset = glm::radians(xpos - m_lastCursorPos.x);
				float yoffset = glm::radians(ypos - m_lastCursorPos.y);

				camera->changeYawPitch(xoffset, yoffset);
			}
			break;
		}

		m_lastCursorPos.x = xpos;
		m_lastCursorPos.y = ypos;
	}
	
	void MainScene::OnMouse(int button, int action, int mods)
	{
		switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			m_leftMouse = (action == GLFW_PRESS);
			break;

		case GLFW_MOUSE_BUTTON_RIGHT:
			m_rightMouse = (action == GLFW_PRESS);
			break;
		}
	}

	void MainScene::OnScroll(double xoffset, double yoffset)
	{
		camera->changeDistance(static_cast<float>(yoffset * 0.01));
	}

	auto MainScene::LoadScene() -> bool
	{
		camera = new Camera();

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