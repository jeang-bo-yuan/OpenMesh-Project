#pragma once

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <optional>

namespace CG
{
	/* Define custom traits */
	struct MyTraits : OpenMesh::DefaultTraits
	{
		// Define vertex and normal as double
		using Point = OpenMesh::Vec3d;
		using Normal = OpenMesh::Vec3d;

		// Add normal property to vertices and faces
		VertexAttributes(OpenMesh::Attributes::Normal);
		FaceAttributes(OpenMesh::Attributes::Normal);
	};

	/**
	 * 模型
	 */
	class MyMesh : public OpenMesh::TriMesh_ArrayKernelT<MyTraits>
	{
		friend class TextureManager;

	public:
		MyMesh();
		~MyMesh();

		GLuint GetSelectedSSBO() { return selectedSSBO; }

		/**
		 * 從檔案載入模型
		 * \param filename - 模型檔的名稱
		 * \return 是否載入成功
		 */
		bool LoadFromFile(std::string filename);

		void Render();

		//! 從 filename 這個檔案載入貼圖座標
		void LoadTexCoord(const std::string& filename);
		//! 將貼圖座標匯出到 filename 這個檔案
		void ExportTexCoord(const std::string& filename);

		/**
		 * 繪製每一面，顏色的 R channel 為面的ID (ID = 陣列中的index + 1)
		 */
		void RenderFaceID();

		/**
		 * 選擇面上最靠近position的點
		 * \param position - 提示點的位置
		 * \param face_index - 選擇哪個面上的點
		 */
		void SelectPoint(glm::vec3 position, GLuint face_index);

		/**
		 * 取消選擇點
		 */
		void UnselectPoint() { selectedPoint = std::nullopt; }

		/**
		 * 選取或取消選取某一面
		 * \param face_index - 這個面在陣列中的index
		 * \param selected - 是否被選中（非0 -> 選取，0 -> 取消選取）
		 */
		void SelectFace(GLuint face_index, int selected);

		/**
		 * 取消選取任何面和點
		 */
		void UnselectAll();

	private:
		// 生成buffer
		void CreateBuffers();
		void CreateShaders();
		//! 計算每條邊的權重並存在edge property中的"weight"
		void CaculateEdgeWeight();
		
		// halfedge, face, and vertex normals
		OpenMesh::Vec3d normal(const HalfedgeHandle he) const;
		OpenMesh::Vec3d normal(const EdgeHandle e) const;
		OpenMesh::Vec3d normal(const FaceHandle f) const;
		OpenMesh::Vec3d normal(const VertexHandle v) const;

		glm::vec3 d2f(OpenMesh::Vec3d v);

	private:
		/// 哪個點被選中了，std::nullopt -> 沒有
		std::optional<MyMesh::VertexHandle> selectedPoint;

		/* Shader Storage Buffer Objects */
		/// 用來儲存哪個面有被選中的SSBO，包含 n_faces() 個 int
		GLuint selectedSSBO;

		/* Buffers for solid rendering */
		GLuint sVAO;
		GLuint sVBOp;
		GLuint sVBOn;
		GLuint sVBOtexcoord; //!< 貼圖座標

		/* Buffers for wireframe rendering */
		GLuint wVAO;
		GLuint wVBOp;
		GLuint wVBOn;

		/* Phong shader */
		GLuint programPhong;
		GLuint pMatVPID;
		GLuint pModelID;
		GLuint pMatKaID;
		GLuint pMatKdID;
		GLuint pMatKsID;

		/* Line shader */
		GLuint programLine;
		GLuint lMatVPID;
		GLuint lModelID;
		GLuint lMatKdID;

		/* Face ID shader */
		GLuint programFaceID;
		GLuint programFaceID_model; //!< uniform location of "model" in program Face ID

		/* select dot shader */
		GLuint programSelectDot;

		/* Model properties */
		glm::mat4 model;
		glm::vec3 colorAmbient;
		glm::vec3 colorDiffuse;
		glm::vec3 colorSpecular;
		glm::vec3 colorLine;

	};

	// ---- data structures ----
	using Point = MyMesh::Point;
	// ---- Handles ----
	using VertexHandle = MyMesh::VertexHandle;
	using HalfedgeHandle = MyMesh::HalfedgeHandle;
	using EdgeHandle = MyMesh::EdgeHandle;
	using FaceHandle = MyMesh::FaceHandle;
	// ---- Iterators ----
	using EdgeIter = MyMesh::EdgeIter;
	using VertexIter = MyMesh::VertexIter;
	using FaceIter = MyMesh::FaceIter;
	using VertexOHalfedgeIter = MyMesh::VertexOHalfedgeIter;
	using VertexVertexIter = MyMesh::VertexVertexIter;
}

