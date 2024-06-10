#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <optional>
#include "MyMesh.h"
#include <Utilty/LoadShaders.h>
#include <vector>

namespace CG {
	/**
	 * 負責管理texture 2d array + 貼圖座標參數化
	 */
	class TextureManager
	{
		// 用來繪製產生的結果
		GLuint m_vao;
		GLuint m_vbo; // 存2D的texture座標
		GLuint m_program;

		GLuint m_texture_array;

		MyMesh* m_origin_mesh_ptr;

		typedef OpenMesh::TriMesh_ArrayKernelT<CG::MyTraits> CopiedMesh_t;
		std::optional<CopiedMesh_t> m_copied_mesh;
		std::vector<CopiedMesh_t::VertexHandle> m_on_boundary;
		std::vector<CopiedMesh_t::VertexHandle> m_inside_boundary;
		// 在繞boundary一圈時要從哪開始
		CopiedMesh_t::HalfedgeHandle m_boundary_start = CopiedMesh_t::InvalidHalfedgeHandle;

	public:
		static constexpr int MAX_IMG = 10;
		static constexpr int IMG_SIZE = 720;

		/**
		 * 建構子
		 * \param mesh - 讓texture manager可以對mesh產生貼圖座標
		 */
		TextureManager(MyMesh* mesh);

		/**
		 * 載入一張圖片到texture array中
		 * \param file - 檔名
		 * \param layer - 層（0 <= layer < MAX_IMG）
		 * \return 是否成功
		 */
		bool LoadImage(const std::string& file, int layer);

		//! bind to texture unit 0
		void BindTextureArray_0();

/////////Texture Parameterization////////////////////////////////////////////////////////////////
	public:
		//! 看哪些面被選中了並為這些面產生貼圖座標
		//! @param layer - 使用哪一張貼圖（-1 <= layer < MAX_IMG）
		void GenTexCoord(int layer);
	private:
		//! 從m_origin_mesh_ptr中將選中的面複製進m_copied_mesh。回傳false，若沒有面被選中；反之回傳true
		bool CopySelectedFaces();
		//! 找到copied mesh中的邊界，為邊界上的點設定texcoord，並將所有點分成兩類（on_boundary和inside_boundary）
		void FindBoundaryAndSplit();
		//! 解線性系統，以求出在邊界內的點的texcoord
		void SolveLinearEquation();
		//! 將算出來的texcoord寫入origin_mesh的VBO
		void WriteResult(int layer);
/////////////////////////////////////////////////////////////////////////////////////////////////

	public:
		//! 隱藏起來，直到 GenTexCoord 時才會再顯示
		void HideResult() { m_copied_mesh = std::nullopt; }

		//! 將結果繪製出來
		void RenderResult();
	};
}