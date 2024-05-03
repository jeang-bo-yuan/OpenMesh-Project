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
		RotateCamera,  //!< �i�H����۾�
		SelectVertex, //!< �i�H��ܳ��I
		SelectFace, //!< �i�H�ﭱ
		UnselectFace, //!< �i�H���������
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

		//! ����ܵ����j�p
		void OnResize(int width, int height);
		//! ����U��L
		void OnKeyboard(int key, int action);
		//! ��ƹ����ʡA�Ѽƪ��y�ХH�u���W���v�����I
		//! @param xpos - x�y�С]�̥��謰0�^
		//! @param ypos - y�y�С]�̤W�謰0�^
		void OnCursorMove(float xpos, float ypos);
		//! ���I���ƹ�
		//! @param button - ���U���ƹ�����
		//! @param action - GLFW_PRESS or GLFW_RELEASE
		//! @param mods - modifier keys
		void OnMouse(int button, int action, int mods);
		//! ��ƹ��u���u��
		//! @param xoffset - �����u��
		//! @param yoffset - �]���u��
		void OnScroll(double xoffset, double yoffset);

		void UnselectAll() { mesh->UnselectAll(); }

	private:
		auto LoadScene() -> bool;

		/**
		 * �إ� FBO �M UBO
		 */
		void CreateBuffers();

		/**
		 * ���o���Цb���ӭ��W
		 * @return ����ID�]0 -> �I���^
		 */
		GLuint MouseOnWhichFace();

		/**
		 * ���o���ЦbScreen Space�����y��
		 */
		glm::vec2 GetCursorWinPos() { 
			return glm::vec2(m_lastCursorPos.x, m_height - m_lastCursorPos.y); // GLFW���̤W�謰0�A��OpenGL�̤U�謰0
		}

		/**
		 * �̾� m_lastCursorPos ����m���or���������
		 * \param selected - �O�_���
		 */
		void SelectFaceWithMouse(bool selected);

		/**
		 * �̾� m_lastCursorPos ����m���or��������I
		 */
		void SelectPointWithMouse();

	private:
		bool m_leftMouse; //!< ����O�_���U
		bool m_rightMouse; //!< �k��O�_���U
		glm::vec2 m_lastCursorPos; //!< �W�����Ъ���m

		State m_current_state = State::RotateCamera;

		int m_width = 0;
		int m_height = 0;
		
		struct {
			GLuint name = 0;
			GLuint color_texture = 0; //!< ���e�G GL_R32UI
			GLuint depth_rbo = 0;
		} m_faceID_fbo;  //!< �N�C�ӭ���index�e�bR channel

		/// binding = 0, matrix UBO (mat4 view, mat4 proj)
		GLuint m_matrix_UBO = 0;
		
		Camera* camera;

		MyMesh* mesh;
	};
}

