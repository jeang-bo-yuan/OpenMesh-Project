#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <optional>
#include "MyMesh.h"
#include <Utilty/LoadShaders.h>
#include <vector>

namespace CG {
	/**
	 * �t�d�޲ztexture 2d array + �K�Ϯy�аѼƤ�
	 */
	class TextureManager
	{
		// �Ψ�ø�s���ͪ����G
		GLuint m_vao;
		GLuint m_vbo; // �s2D��texture�y��
		GLuint m_program;

		GLuint m_texture_array;

		MyMesh* m_origin_mesh_ptr;

		typedef OpenMesh::TriMesh_ArrayKernelT<CG::MyTraits> CopiedMesh_t;
		std::optional<CopiedMesh_t> m_copied_mesh;
		std::vector<CopiedMesh_t::VertexHandle> m_on_boundary;
		std::vector<CopiedMesh_t::VertexHandle> m_inside_boundary;
		// �b¶boundary�@��ɭn�q���}�l
		CopiedMesh_t::HalfedgeHandle m_boundary_start = CopiedMesh_t::InvalidHalfedgeHandle;

	public:
		static constexpr int MAX_IMG = 10;
		static constexpr int IMG_SIZE = 720;

		/**
		 * �غc�l
		 * \param mesh - ��texture manager�i�H��mesh���ͶK�Ϯy��
		 */
		TextureManager(MyMesh* mesh);

		/**
		 * ���J�@�i�Ϥ���texture array��
		 * \param file - �ɦW
		 * \param layer - �h�]0 <= layer < MAX_IMG�^
		 * \return �O�_���\
		 */
		bool LoadImage(const std::string& file, int layer);

		//! bind to texture unit 0
		void BindTextureArray_0();

/////////Texture Parameterization////////////////////////////////////////////////////////////////
	public:
		//! �ݭ��ǭ��Q�襤�F�ì��o�ǭ����ͶK�Ϯy��
		//! @param layer - �ϥέ��@�i�K�ϡ]-1 <= layer < MAX_IMG�^
		void GenTexCoord(int layer);
	private:
		//! �qm_origin_mesh_ptr���N�襤�����ƻs�im_copied_mesh�C�^��false�A�Y�S�����Q�襤�F�Ϥ��^��true
		bool CopySelectedFaces();
		//! ���copied mesh������ɡA����ɤW���I�]�wtexcoord�A�ñN�Ҧ��I���������]on_boundary�Minside_boundary�^
		void FindBoundaryAndSplit();
		//! �ѽu�ʨt�ΡA�H�D�X�b��ɤ����I��texcoord
		void SolveLinearEquation();
		//! �N��X�Ӫ�texcoord�g�Jorigin_mesh��VBO
		void WriteResult(int layer);
/////////////////////////////////////////////////////////////////////////////////////////////////

	public:
		//! ���ð_�ӡA���� GenTexCoord �ɤ~�|�A���
		void HideResult() { m_copied_mesh = std::nullopt; }

		//! �N���Gø�s�X��
		void RenderResult();
	};
}