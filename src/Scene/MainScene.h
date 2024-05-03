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
		SelectVertex, //!< 可以選擇頂點
		SelectFace, //!< 可以選面
		UnselectFace, //!< 可以取消選取面
	};

	class MainScene
	{
	public:
		MainScene();
		~MainScene();

		auto Initialize() -> bool;
		void Update(double dt);
		void Render();

		State GetState() const { return m_current_state; }
		void SetState(State new_state) { m_current_state = new_state; }

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

		void UnselectAll() { mesh->UnselectAll(); }

	private:
		auto LoadScene() -> bool;

		/**
		 * 建立 FBO 和 UBO
		 */
		void CreateBuffers();

		/**
		 * 取得鼠標在哪個面上
		 * @return 面的ID（0 -> 背景）
		 */
		GLuint MouseOnWhichFace();

		/**
		 * 取得鼠標在Screen Space中的座標
		 */
		glm::vec2 GetCursorWinPos() { 
			return glm::vec2(m_lastCursorPos.x, m_height - m_lastCursorPos.y); // GLFW中最上方為0，但OpenGL最下方為0
		}

		/**
		 * 依據 m_lastCursorPos 的位置選取or取消選取面
		 * \param selected - 是否選取
		 */
		void SelectFaceWithMouse(bool selected);

		/**
		 * 依據 m_lastCursorPos 的位置選取or取消選取點
		 */
		void SelectPointWithMouse();

	private:
		bool m_leftMouse; //!< 左鍵是否按下
		bool m_rightMouse; //!< 右鍵是否按下
		glm::vec2 m_lastCursorPos; //!< 上次鼠標的位置

		State m_current_state = State::RotateCamera;

		int m_width = 0;
		int m_height = 0;
		
		struct {
			GLuint name = 0;
			GLuint color_texture = 0; //!< 內容： GL_R32UI
			GLuint depth_rbo = 0;
		} m_faceID_fbo;  //!< 將每個面的index畫在R channel

		/// binding = 0, matrix UBO (mat4 view, mat4 proj)
		GLuint m_matrix_UBO = 0;
		
		Camera* camera;

		MyMesh* mesh;
	};
}

