#pragma once
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <optional>
#include <Utilty/LoadShaders.h>

namespace CG {
	// forward declaration
	class MyMesh;

	/**
	 * 負責管理texture 2d array + 貼圖座標參數化
	 */
	class TextureManager
	{
		// 用來繪製產生的結果
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
		 * 載入一張圖片到texture array中
		 * \param file - 檔名
		 * \param layer - 層（0 <= layer < MAX_IMG）
		 * \return 是否成功
		 */
		bool LoadImage(const std::string& file, int layer);

		//! bind to texture unit 0
		void BindTextureArray_0();

		/**
		 * 看the_mesh中哪些面被選中了並為這些面產生貼圖座標
		 * \param the_mesh
		 */
		//void GenerateTextureCoordinate(MyMesh& the_mesh);

		////! 隱藏起來，直到 GenerateTextureCoordinate 時才會再顯示
		//void HideResult() { m_show_parameterization = false; }

		//void RenderResult();
	};
}