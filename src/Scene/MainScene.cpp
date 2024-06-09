
#include "MainScene.h"
#include <assert.h>

namespace CG
{
	MainScene::MainScene()
		: m_leftMouse(false), m_rightMouse(false), m_lastCursorPos()
	{
		camera = nullptr;
		mesh = nullptr;
		m_tex_manager = nullptr;
	}

	MainScene::~MainScene()
	{
	}

	auto MainScene::Initialize() -> bool
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback([](GLenum source​, GLenum type​, GLuint id​, GLenum severity​, GLsizei length​, const GLchar* message​, const void* userParam​) {
			std::cout << "=======================================================\n";
			std::cout << "Source: " << (source​ == GL_DEBUG_SOURCE_API ? "API" :
				source​ == GL_DEBUG_SOURCE_WINDOW_SYSTEM ? "Window System" :
				source​ == GL_DEBUG_SOURCE_SHADER_COMPILER ? "Shader Compiler" :
				source​ == GL_DEBUG_SOURCE_THIRD_PARTY ? "Third Party" :
				source​ == GL_DEBUG_SOURCE_APPLICATION ? "Application" : "Other");

			std::cout << "\nType: " << (type​ == GL_DEBUG_TYPE_ERROR ? "Error" :
				type​ == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR ? "Deprecated" :
				type​ == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR ? "Undefined Behavior" :
				type​ == GL_DEBUG_TYPE_PORTABILITY ? "Portability" :
				type​ == GL_DEBUG_TYPE_PERFORMANCE ? "Performance" :
				type​ == GL_DEBUG_TYPE_MARKER ? "Marker" :
				type​ == GL_DEBUG_TYPE_PUSH_GROUP ? "Push Group" :
				type​ == GL_DEBUG_TYPE_POP_GROUP ? "Pop Group" : "Other");

			std::cout << "\nSeverity: " << (severity​ == GL_DEBUG_SEVERITY_HIGH ? "High" :
				severity​ == GL_DEBUG_SEVERITY_MEDIUM ? "Medium" :
				severity​ == GL_DEBUG_SEVERITY_LOW ? "Low" : "Notification");

			std::cout << "\n(ID " << id​ << ") " << message​;
			std::cout << "\n=======================================================\n" << std::flush;

			// don't output same message twice
			if (severity​ != GL_DEBUG_SEVERITY_HIGH)
				glDebugMessageControl(source​, type​, GL_DONT_CARE, 1, &id​, GL_FALSE);
		}, nullptr);

		CreateBuffers();
		return LoadScene();
	}

	void MainScene::Update(double dt)
	{

	}

	void MainScene::Render()
	{
		// update UBO
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_matrix_UBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->GetViewMatrix()));
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->GetProjectionMatrix()));

		glClearColor(0.0, 0.0, 0.0, 1); //black screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_tex_manager->BindTextureArray_0();

		mesh->Render();

		// 繪製每一面的ID
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_faceID_fbo.name);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mesh->RenderFaceID();

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// 繪製選取範圍
		if ((m_current_state == State::RegionSelectFace || m_current_state == State::RegionUnselectFace)
			&& m_leftMouse) {
			m_selection_region.Render(GetCursorNDCPos());
		}
		if (m_renderAndSelectInRegion) {
			std::cout << "Select in region" << std::endl;
			m_selection_region.RenderAndSelect(GetCursorNDCPos(), m_faceID_fbo.color_texture, mesh->GetSelectedSSBO(), m_current_state == State::RegionSelectFace);
			m_renderAndSelectInRegion = false;
		}
	}

	void MainScene::OnResize(int width, int height)
	{
		std::cout << "MainScene Resize: " << width << " " << height << std::endl;
		if (width == 0 || height == 0) return;

		m_width = width; m_height = height;
		camera->SetAspect((float)width / height);

		// 重新配置和視窗同大小的 rbo
		glDeleteRenderbuffers(1, &m_faceID_fbo.depth_rbo);
		glGenRenderbuffers(1, &m_faceID_fbo.depth_rbo);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_faceID_fbo.name);
		// color attachment 0
		glBindTexture(GL_TEXTURE_2D, m_faceID_fbo.color_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_faceID_fbo.color_texture, 0);
		// depth attachment
		glBindRenderbuffer(GL_RENDERBUFFER, m_faceID_fbo.depth_rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_faceID_fbo.depth_rbo);

#ifndef NDEBUG
		// check FBO
		if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "FBO is complete" << std::endl;
		}
		else {
			std::cout << "FBO is NOT complete : " << glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) << std::endl;
		}
#endif
		
		// unbind
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		m_selection_region.SetViewport(width, height);
	}

	void MainScene::OnKeyboard(int key, int action)
	{
#ifndef NDEBUG
		std::cout << "MainScene OnKeyboard: " << key << " " << action << std::endl;
#endif

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

		if (action == GLFW_PRESS) {
			if (key == GLFW_KEY_1) {
				this->SetState(State::RotateCamera);
			}
			else if (key == GLFW_KEY_2) {
				this->SetState(State::SelectVertex);
			}
			else if (key == GLFW_KEY_3) {
				this->SetState(State::SelectFace);
			}
			else if (key == GLFW_KEY_4) {
				this->SetState(State::UnselectFace);
			}
			else if (key == GLFW_KEY_5) {
				this->SetState(State::RegionSelectFace);
			}
			else if (key == GLFW_KEY_6) {
				this->SetState(State::RegionUnselectFace);
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

		case State::SelectFace: case State::UnselectFace:
			if (m_leftMouse)
				SelectFaceWithMouse(m_current_state == State::SelectFace);
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
			// 若左鍵有按下
			if (m_leftMouse) {
				// 選面
				if (m_current_state == State::SelectFace || m_current_state == State::UnselectFace) {
					SelectFaceWithMouse(m_current_state == State::SelectFace);
				}
				// 選點
				else if (m_current_state == State::SelectVertex) {
					SelectPointWithMouse();
				}
			}

			// 範圍選取
			if (m_current_state == State::RegionSelectFace || m_current_state == State::RegionUnselectFace) {
				if (m_leftMouse) {
					m_selection_region.SetRegionStart(GetCursorNDCPos());
				}
				else {
					// 鬆開滑鼠時要選取
					m_renderAndSelectInRegion = true;
				}
			}
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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDepthMask(GL_TRUE);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

		mesh = new MyMesh();
		mesh->LoadFromFile("./res/models/xyzrgb_dragon_100k.obj");

		m_tex_manager = new TextureManager(mesh);
		
		return true;
	}

	void MainScene::CreateBuffers()
	{
		// create FBO
		glGenFramebuffers(1, &m_faceID_fbo.name);
		glGenTextures(1, &m_faceID_fbo.color_texture);

		// create UBO
		glGenBuffers(1, &m_matrix_UBO);
		glBindBuffer(GL_UNIFORM_BUFFER, m_matrix_UBO);
		glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	GLuint MainScene::MouseOnWhichFace()
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_faceID_fbo.name);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glViewport(0, 0, m_width, m_height);

		glm::vec2 winPos = GetCursorWinPos();

		GLuint ID = 0;
		// 圾垃 reference，竟然沒說要用GL_RED_INTEGER才能從integer texture讀像素，害我Debug 3小時
		// solution: https://stackoverflow.com/a/55141849/20876404
		glReadPixels(winPos.x, winPos.y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &ID);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		return ID;
	}

	glm::vec2 MainScene::GetCursorNDCPos()
	{
		glm::vec2 result = GetCursorWinPos();
		float half_width = m_width / 2.f;
		float half_height = m_height / 2.f;

		result.x = (result.x - half_width) / half_width;
		result.y = (result.y - half_height) / half_height;

		return result;
	}

	void MainScene::SelectFaceWithMouse(bool selected)
	{
		GLuint faceID = MouseOnWhichFace();
		
		if (faceID != 0) {
#ifndef NDEBUG
			std::cout << "[] Face: " << faceID - 1 << std::endl;
#endif
			mesh->SelectFace(faceID - 1, selected);
		}
	}

	void MainScene::SelectPointWithMouse()
	{
		GLuint faceID = MouseOnWhichFace();
		
		if (faceID == 0) {
			mesh->UnselectPoint();
		}
		else {
			glm::vec2 winPos = GetCursorWinPos();
			GLfloat depth = 0;
			glReadPixels(winPos.x, winPos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

			// Unproject cursorPos to world pos
			glm::vec3 world_pos = glm::unProject(glm::vec3(winPos.x, winPos.y, depth), // win pos
				camera->GetViewMatrix(), 
				camera->GetProjectionMatrix(), 
				glm::vec4(0, 0, m_width, m_height)); // viewport

			mesh->SelectPoint(world_pos, faceID - 1);
		}
	}
}