
#include "MainScene.h"
#include <assert.h>

namespace CG
{
	MainScene::MainScene()
	{
		camera = nullptr;
		mesh = nullptr;

		assert(std::get<0>(m_state_data).rotating == false);
		assert(std::get<0>(m_state_data).recordCursor == false);
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
	}

	void MainScene::OnCursorMove(double xpos, double ypos)
	{
		switch (m_current_state) {

		case State::RotateCamera:
			// �b���त
			if (std::get<0>(m_state_data).rotating) {
				// �ǳƶ��q
				if (std::get<0>(m_state_data).recordCursor) {
					std::cout << "MainScene: Start Rotating Camera" << std::endl;

					// �O�U���Ц�m
					std::get<0>(m_state_data).lastCursorPos = glm::dvec2(xpos, ypos);
					// �����ǳƶ��q
					std::get<0>(m_state_data).recordCursor = false;
				}
				else {
					double xoffset = xpos - std::get<0>(m_state_data).lastCursorPos.x;
					double yoffset = ypos - std::get<0>(m_state_data).lastCursorPos.y;
					xoffset = glm::radians(xoffset);
					yoffset = glm::radians(yoffset);
					//std::cout << "(deltaYaw, deltaPitch) = (" << xoffset << ", " << yoffset << ")" << std::endl;

					// ��ʬ۾�
					camera->changeYawPitch(xoffset, yoffset);

					// �O�U���Ц�m
					std::get<0>(m_state_data).lastCursorPos = glm::dvec2(xpos, ypos);
				}
			}
			break;

		}
	}
	
	void MainScene::OnMouse(int button, int action, int mods)
	{
		switch (m_current_state) {

		case State::RotateCamera:
			if (button == GLFW_MOUSE_BUTTON_LEFT) {
				bool button_press = (action == GLFW_PRESS);
				std::get<0>(m_state_data).rotating = button_press;
				std::get<0>(m_state_data).recordCursor = button_press;
			}
			break;

		}
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