#pragma once

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

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

	class MyMesh : public OpenMesh::TriMesh_ArrayKernelT<MyTraits>
	{
	public:
		MyMesh();
		~MyMesh();

		bool LoadFromFile(std::string filename);

		void Render();

		/**
		 * 繪製每一面，顏色的 R channel 為面的 index
		 */
		void RenderFaceID();

	private:
		void CreateBuffers();
		void CreateShaders();
		
		// halfedge, face, and vertex normals
		OpenMesh::Vec3d normal(const HalfedgeHandle he) const;
		OpenMesh::Vec3d normal(const EdgeHandle e) const;
		OpenMesh::Vec3d normal(const FaceHandle f) const;
		OpenMesh::Vec3d normal(const VertexHandle v) const;

		glm::vec3 d2f(OpenMesh::Vec3d v);

	private:
		/* Buffers for solid rendering */
		GLuint sVAO;
		GLuint sVBOp;
		GLuint sVBOn;

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

