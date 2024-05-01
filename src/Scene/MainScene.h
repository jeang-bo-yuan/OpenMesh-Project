#pragma once

#include <array>
#include <string>
#include <map>
#include <vector>
#include <variant>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include <Mesh/MyMesh.h>

constexpr auto PARTSNUM = 18;

namespace CG
{
	enum class State {
		RotateCamera,  //!< 可以旋轉相機
	};

	class MainScene
	{
	public:
		MainScene();
		~MainScene();

		auto Initialize() -> bool;
		void Update(double dt);
		void Render();

		//! 當改變視窗大小
		void OnResize(int width, int height);
		//! 當按下鍵盤
		void OnKeyboard(int key, int action);
		//! 當滑鼠移動，參數的座標以「左上角」為原點
		//! @param xpos - x座標（最左方為0）
		//! @param ypos - y座標（最上方為0）
		void OnCursorMove(float xpos, float ypos);
		//! 當點擊滑鼠
		//! @param button - 按下的滑鼠按鍵
		//! @param action - GLFW_PRESS or GLFW_RELEASE
		//! @param mods - modifier keys
		void OnMouse(int button, int action, int mods);
		//! 當滑鼠滾輪滾動
		//! @param xoffset - 水平滾動
		//! @param yoffset - 鉛直滾動
		void OnScroll(double xoffset, double yoffset);

	private:
		auto LoadScene() -> bool;

	private:
		bool m_leftMouse; //!< 左鍵是否按下
		bool m_rightMouse; //!< 右鍵是否按下
		glm::vec2 m_lastCursorPos; //!< 上次鼠標的位置

		State m_current_state = State::RotateCamera;

		Camera* camera;

		MyMesh* mesh;
	};
}

