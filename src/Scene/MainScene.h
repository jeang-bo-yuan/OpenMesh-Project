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
	};

	//! �� State == RotateCamera �ɪ����
	struct RotateCameraData {
		bool rotating = false;
		bool recordCursor = false;
		glm::dvec2 lastCursorPos;
	};

	class MainScene
	{
	public:
		MainScene();
		~MainScene();

		auto Initialize() -> bool;
		void Update(double dt);
		void Render();

		//! ����ܵ����j�p
		void OnResize(int width, int height);
		//! ����U��L
		void OnKeyboard(int key, int action);
		//! ��ƹ����ʡA�Ѽƪ��y�ХH�u���W���v�����I
		//! @param xpos - x�y�С]�̥��謰0�^
		//! @param ypos - y�y�С]�̤W�謰0�^
		void OnCursorMove(double xpos, double ypos);
		//! ���I���ƹ�
		//! @param button - ���U���ƹ�����
		//! @param action - GLFW_PRESS or GLFW_RELEASE
		//! @param mods - modifier keys
		void OnMouse(int button, int action, int mods);
		//! ��ƹ��u���u��
		//! @param xoffset - �����u��
		//! @param yoffset - �]���u��
		void OnScroll(double xoffset, double yoffset);

	private:
		auto LoadScene() -> bool;

	private:
		State m_current_state = State::RotateCamera;
		std::variant<RotateCameraData> m_state_data;

		Camera* camera;

		MyMesh* mesh;
	};
}

