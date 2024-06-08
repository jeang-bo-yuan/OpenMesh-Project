#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <optional>
#include <Utilty/LoadShaders.h>

namespace CG {
	// forward declaration
	class MyMesh;

	/**
	 * �t�d�޲ztexture 2d array + �K�Ϯy�аѼƤ�
	 */
	class TextureManager
	{
		// �Ψ�ø�s���ͪ����G
		/*GLuint m_vao;
		GLuint m_vbo;
		GLuint m_ebo;
		GLuint m_program;*/
		GLuint m_texture_array;
		bool m_show_parameterization = false;

	public:
		static constexpr int MAX_IMG = 10;
		static constexpr int IMG_SIZE = 720;

		TextureManager();

		/**
		 * ���J�@�i�Ϥ���texture array��
		 * \param file - �ɦW
		 * \param layer - �h�]0 <= layer < MAX_IMG�^
		 * \return �O�_���\
		 */
		bool LoadImage(const std::string& file, int layer);

		//! bind to texture unit 0
		void BindTextureArray_0();

		/**
		 * ��the_mesh�����ǭ��Q�襤�F�ì��o�ǭ����ͶK�Ϯy��
		 * \param the_mesh
		 */
		//void GenerateTextureCoordinate(MyMesh& the_mesh);

		////! ���ð_�ӡA���� GenerateTextureCoordinate �ɤ~�|�A���
		//void HideResult() { m_show_parameterization = false; }

		//void RenderResult();
	};
}