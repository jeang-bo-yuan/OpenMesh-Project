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
	 * �ҫ�
	 */
	class MyMesh : public OpenMesh::TriMesh_ArrayKernelT<MyTraits>
	{
		friend class TextureManager;

	public:
		MyMesh();
		~MyMesh();

		GLuint GetSelectedSSBO() { return selectedSSBO; }

		/**
		 * �q�ɮ׸��J�ҫ�
		 * \param filename - �ҫ��ɪ��W��
		 * \return �O�_���J���\
		 */
		bool LoadFromFile(std::string filename);

		void Render();

		//! �q filename �o���ɮ׸��J�K�Ϯy��
		void LoadTexCoord(const std::string& filename);
		//! �N�K�Ϯy�жץX�� filename �o���ɮ�
		void ExportTexCoord(const std::string& filename);

		/**
		 * ø�s�C�@���A�C�⪺ R channel ������ID (ID = �}�C����index + 1)
		 */
		void RenderFaceID();

		/**
		 * ��ܭ��W�̾a��position���I
		 * \param position - �����I����m
		 * \param face_index - ��ܭ��ӭ��W���I
		 */
		void SelectPoint(glm::vec3 position, GLuint face_index);

		/**
		 * ��������I
		 */
		void UnselectPoint() { selectedPoint = std::nullopt; }

		/**
		 * ����Ψ�������Y�@��
		 * \param face_index - �o�ӭ��b�}�C����index
		 * \param selected - �O�_�Q�襤�]�D0 -> ����A0 -> ��������^
		 */
		void SelectFace(GLuint face_index, int selected);

		/**
		 * ����������󭱩M�I
		 */
		void UnselectAll();

	private:
		// �ͦ�buffer
		void CreateBuffers();
		void CreateShaders();
		//! �p��C���䪺�v���æs�bedge property����"weight"
		void CaculateEdgeWeight();
		
		// halfedge, face, and vertex normals
		OpenMesh::Vec3d normal(const HalfedgeHandle he) const;
		OpenMesh::Vec3d normal(const EdgeHandle e) const;
		OpenMesh::Vec3d normal(const FaceHandle f) const;
		OpenMesh::Vec3d normal(const VertexHandle v) const;

		glm::vec3 d2f(OpenMesh::Vec3d v);

	private:
		/// �����I�Q�襤�F�Astd::nullopt -> �S��
		std::optional<MyMesh::VertexHandle> selectedPoint;

		/* Shader Storage Buffer Objects */
		/// �Ψ��x�s���ӭ����Q�襤��SSBO�A�]�t n_faces() �� int
		GLuint selectedSSBO;

		/* Buffers for solid rendering */
		GLuint sVAO;
		GLuint sVBOp;
		GLuint sVBOn;
		GLuint sVBOtexcoord; //!< �K�Ϯy��

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

