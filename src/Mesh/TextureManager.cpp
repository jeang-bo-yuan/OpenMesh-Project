#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <OpenMesh/Core/Utils/PropertyManager.hh>
#include <Eigen/Core>
#include <Eigen/Sparse>
#include <map>

CG::TextureManager::TextureManager(MyMesh* mesh) : m_origin_mesh_ptr(mesh), m_copied_mesh(std::nullopt)
{
	glGenTextures(1, &m_texture_array);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, IMG_SIZE, IMG_SIZE, MAX_IMG);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);

	ShaderInfo shader[] = {
		{ GL_VERTEX_SHADER, "./res/shaders/texture_plane.vert" },
		{ GL_FRAGMENT_SHADER, "./res/shaders/texture_plane.frag" },
		{ GL_NONE, NULL }
	};
	m_program = LoadShaders(shader);

	glUseProgram(m_program);
	glm::mat4 proj_matrix = glm::ortho<float>(-0.05, 1.05, -0.05, 1.05, -1, 1);
	glUniformMatrix4fv(glGetUniformLocation(m_program, "proj_matrix"), 1, GL_FALSE, glm::value_ptr(proj_matrix));
	glUseProgram(0);
}

bool CG::TextureManager::LoadImage(const std::string& file, int layer)
{
	if (layer < 0 || layer >= MAX_IMG) {
		std::cerr << "\x1B[31mInvalid `layer` for TextureManager::LoadImage()\n\x1B[m";
		return false;
	}

	int width = 0, height = 0, channel = 0;
	unsigned char* img = stbi_load(file.c_str(), &width, &height, &channel, 3);

	if (img == nullptr) {
		std::cerr << "\x1B[31mCannot Load Image: " << file << "\n\x1B[m";
		return false;
	}

	if (width != IMG_SIZE || height != IMG_SIZE) {
		std::cerr << "\x1B[31mImage Size Should be " << IMG_SIZE << " * " << IMG_SIZE << "\n\x1B[m";
		return false;
	}

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, layer, IMG_SIZE, IMG_SIZE, 1, GL_RGB, GL_UNSIGNED_BYTE, img);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	std::cout << "Success to load the image" << std::endl;
	return true;
}

void CG::TextureManager::BindTextureArray_0()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_texture_array);
}

void CG::TextureManager::GenTexCoord(int layer)
{
	std::cout << "\n\x1B[32mGenerate Texture Coordinate with layer = " << layer << " ...\x1B[m" << std::endl;

	if (layer < -1 || layer >= MAX_IMG) {
		std::cerr << "\x1B[31m== Invalid argument `layer` (with value = " << layer << ") for TextureManager::GenTexCoord()\x1B[m\n\n";
		return;
	}

	if (CopySelectedFaces() == false) {
		m_copied_mesh = std::nullopt;
		std::cerr << "\x1B[31m== Cannot generate texture coordinate when no faces are selected.\x1B[m\n\n";
		return;
	}

	try {
		FindBoundaryAndSplit();
		SolveLinearEquation();
		WriteResult(layer);
	}
	catch (std::runtime_error& ex) {
		std::cerr << "\x1B[31m== " << ex.what() << "\x1B[m\n\n";
		return;
	}

	std::cout << "\x1B[32mGenerating done!!\x1B[m\n\n" << std::flush;
}

bool CG::TextureManager::CopySelectedFaces()
{
	m_copied_mesh.emplace();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_origin_mesh_ptr->selectedSSBO);
	int* selected_face_arr = reinterpret_cast<int*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

	auto ORIGIN_mapped_vertex = OpenMesh::VProp<int>(-1, *m_origin_mesh_ptr); // (temporary) �C���I������copied mesh���������I�A-1�N��S��
	auto ORIGIN_weight = OpenMesh::EProp<float>(*m_origin_mesh_ptr, "weight"); // origin mesh���C���䪺�v��
	auto COPIED_face_origin = OpenMesh::FProp<int>(*m_copied_mesh, "face_origin"); // copied mesh���C�ӭ�������origin mesh�����ӭ�
	auto COPIED_weight = OpenMesh::EProp<float>(*m_copied_mesh, "weight"); // copied mesh���C���䪺�v��
	m_copied_mesh->request_vertex_texcoords2D();
	
	std::vector<CopiedMesh_t::VertexHandle> new_face_vhandles; // vertex handles of a new copied face

	// for each selected face, copy it
	for (int ORIGIN_face_index = 0; ORIGIN_face_index < m_origin_mesh_ptr->n_faces(); ++ORIGIN_face_index) {
		if (selected_face_arr[ORIGIN_face_index] == false) 
			continue; // not selected, ignore

		new_face_vhandles.clear();

		/* circulate three vertices�A�ƻs�@���A�M��Ncopied_mesh����vertex handle�s�inew_face_vhandles */ {
			auto it = m_origin_mesh_ptr->cfv_ccwbegin(m_origin_mesh_ptr->face_handle(ORIGIN_face_index));
			for (; it.is_valid(); ++it) {
				// vertex is not added
				if (ORIGIN_mapped_vertex[*it] == -1) {
					auto point = m_origin_mesh_ptr->point(*it);
					auto vh = m_copied_mesh->add_vertex(point);
#ifndef NDEBUG
					std::cout << "[" << vh <<  "] Vertex Copied: " << point << std::endl;
#endif
					ORIGIN_mapped_vertex[*it] = vh.idx(); // record
				}

				auto vh = m_copied_mesh->vertex_handle(ORIGIN_mapped_vertex[*it]);
				new_face_vhandles.push_back(vh);
			}
		}

		/* add the face */ 
		auto new_fh = m_copied_mesh->add_face(new_face_vhandles);
		COPIED_face_origin[new_fh] = ORIGIN_face_index;
#ifndef NDEBUG
		std::cout << "== Face(" << ORIGIN_face_index << ") Copied with vertices: " << new_face_vhandles[0] << ' ' << new_face_vhandles[1] << ' ' << new_face_vhandles[2] << std::endl;
#endif

		/* copy edge weights */ {
			auto ORIGIN_edge_it = m_origin_mesh_ptr->cfe_ccwbegin(m_origin_mesh_ptr->face_handle(ORIGIN_face_index));
			auto COPIED_edge_it = m_copied_mesh->cfe_ccwbegin(new_fh);
			for (; ORIGIN_edge_it.is_valid(); ++ORIGIN_edge_it, ++COPIED_edge_it) {
				COPIED_weight[*COPIED_edge_it] = ORIGIN_weight[*ORIGIN_edge_it];
			}
		}
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// �M��ӱq����halfedge�}�l¶boundary�@�� /////////////////////////////////////////////////////////////////
	m_boundary_start = CopiedMesh_t::InvalidHalfedgeHandle; // ��invalidate
	const auto& ORIGIN_selected_point = m_origin_mesh_ptr->selectedPoint;
	if (ORIGIN_selected_point.has_value() && ORIGIN_mapped_vertex[*ORIGIN_selected_point] != -1) { // �Y���襤�@�I �B ���I��b�襤�����W
		// �襤�����I������copied mesh�������I
		CopiedMesh_t::VertexHandle mapped_vhandle = m_copied_mesh->vertex_handle(ORIGIN_mapped_vertex[*ORIGIN_selected_point]);

		if (m_copied_mesh->is_boundary(mapped_vhandle)) {
			std::cout << "\x1B[33mUsing custom origin point...\x1B[m" << std::endl;

			for (auto it = m_copied_mesh->voh_begin(mapped_vhandle); it.is_valid(); ++it) {
				if (m_copied_mesh->is_boundary(*it)) {
					m_boundary_start = *it;
					break;
				}
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////

	return m_copied_mesh->n_faces() != 0;
}

template<typename T>
static typename T::HalfedgeHandle first_boundary_heh(T& mesh) {
	for (auto it = mesh.halfedges_sbegin(); it != mesh.halfedges_end(); ++it) {
		if (mesh.is_boundary(*it))
			return *it;
	}

	throw std::runtime_error("No boundary");
}

void CG::TextureManager::FindBoundaryAndSplit()
{
	std::cout << "\x1B[32mFinding Boundary...\x1B[m" << std::endl;

	float total_length = 0.f;
	// �Y���]�w�_�l��halfedge�h�γ]�w���A�_�h�H����
	const CopiedMesh_t::HalfedgeHandle init_heh = (m_boundary_start.is_valid() ? m_boundary_start : first_boundary_heh(*m_copied_mesh));

	// ¶boundary�@��A�p��total_length
	for (auto curr = init_heh; curr != init_heh || total_length == 0.f; curr = m_copied_mesh->next_halfedge_handle(curr)) {
#ifndef NDEBUG
		std::cout << m_copied_mesh->from_vertex_handle(curr) << ' ';
#endif
		total_length += m_copied_mesh->calc_edge_length(curr);
	}
#ifndef NDEBUG
	std::cout << "are on boundary" << std::endl;
#endif

	// �]�wtexCoord�]�Ĥ@�ӡ^
	const CopiedMesh_t::VertexHandle init_vh = m_copied_mesh->from_vertex_handle(init_heh);
	m_copied_mesh->set_texcoord2D(init_vh, CG::MyTraits::TexCoord2D(0, 0));
	// ��L��
	float accumulate_len = m_copied_mesh->calc_edge_length(init_heh);
	for (auto curr = m_copied_mesh->next_halfedge_handle(init_heh); curr != init_heh; curr = m_copied_mesh->next_halfedge_handle(curr)) {
		float t = accumulate_len * 4.f / total_length; // 0 < t < 4
		CG::MyTraits::TexCoord2D result(0, 0);
		if (t <= 1.f)
			result = CG::MyTraits::TexCoord2D(t      , 0.f);
		else if (t <= 2.f)
			result = CG::MyTraits::TexCoord2D(1.f    , t - 1.f);
		else if (t <= 3.f)
			result = CG::MyTraits::TexCoord2D(3.f - t, 1.f);
		else
			result = CG::MyTraits::TexCoord2D(0.f    , 4.f - t);

#ifndef NDEBUG
		std::cout << result << std::endl;
#endif

		auto curr_from = m_copied_mesh->from_vertex_handle(curr);
		m_copied_mesh->set_texcoord2D(curr_from, result);
		accumulate_len += m_copied_mesh->calc_edge_length(curr);
	}

	// split
	m_on_boundary.clear();
	m_inside_boundary.clear();
	for (auto it = m_copied_mesh->vertices_sbegin(); it != m_copied_mesh->vertices_end(); ++it) {
		if (m_copied_mesh->is_boundary(*it))
			m_on_boundary.push_back(*it);
		else
			m_inside_boundary.push_back(*it);
	}
}

void CG::TextureManager::SolveLinearEquation()
{
	std::cout << "\x1B[32mSolving Linear Equation...\x1B[m" << std::endl;

	if (m_inside_boundary.size() == 0) return;

	auto COPIED_weight = OpenMesh::EProp<float>(*m_copied_mesh, "weight");
	Eigen::SparseMatrix<float> A(m_inside_boundary.size(), m_inside_boundary.size());
	Eigen::MatrixXf B; 
	B.resize(m_inside_boundary.size(), 2);
	B.fill(0);

	// �Ϭd��A�ΨӬd��vertex handle�binside_boundary��index
	std::map<CopiedMesh_t::VertexHandle, size_t> inside_reverse_lookup;
	for (size_t i = 0; i < m_inside_boundary.size(); ++i) {
		inside_reverse_lookup[m_inside_boundary[i]] = i;
	}

	// for each vertex inside the boundary
	for (size_t i = 0; i < m_inside_boundary.size(); ++i) {
		float total_weight = 0.f;
		auto curr_vh = m_inside_boundary[i];

		// for each outgoing halfedge
		for (auto oh_it = m_copied_mesh->voh_begin(curr_vh); oh_it.is_valid(); ++oh_it) {
			auto v2_handle = m_copied_mesh->to_vertex_handle(*oh_it); // v2 �� handle
			auto curr_eh = m_copied_mesh->edge_handle(*oh_it); // �ثe�b����
			float weight = COPIED_weight[curr_eh]; // �䪺�v��

			total_weight += weight;

			// �p�G v2 ���b m_inside_boundary ���]i.e. ����texcoord�O�w�����^
			if (inside_reverse_lookup.find(v2_handle) == inside_reverse_lookup.end()) {
				auto v2_texcoord = m_copied_mesh->texcoord2D(v2_handle); // v2 ���K�Ϯy��
				B(i, 0) += weight * v2_texcoord[0];
				B(i, 1) += weight * v2_texcoord[1];
			}
			// v2 �b m_inside_boundary ��
			else {
				size_t v2_idx = inside_reverse_lookup[v2_handle];
				A.insert(i, v2_idx) = -weight;
			}
		}

		A.insert(i, i) = total_weight;
	}

	A.makeCompressed();
	Eigen::SparseQR<decltype(A), Eigen::COLAMDOrdering<int>> linearSolver;
	linearSolver.compute(A);

	Eigen::MatrixXf X;
	X.resize(m_inside_boundary.size(), 2);
	X = linearSolver.solve(B);

	// store X into texcoord2D property
	for (size_t i = 0; i < m_inside_boundary.size(); ++i) {
		auto vh = m_inside_boundary[i];
		CG::MyTraits::TexCoord2D result(X(i, 0), X(i, 1));
		m_copied_mesh->set_texcoord2D(vh, result);
#ifndef NDEBUG
		std::cout << vh << " -> " << result << std::endl;
#endif
	}
}

void CG::TextureManager::WriteResult(int layer)
{
	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "layer"), layer);
	glUseProgram(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_origin_mesh_ptr->sVBOtexcoord);
	glm::vec3* ORIGIN_texcoord_vbo = reinterpret_cast<glm::vec3*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	auto COPIED_face_origin = OpenMesh::FProp<int>(*m_copied_mesh, "face_origin");
	std::vector<glm::vec2> vertices_texcoord; // �O���C���I��texcoord

	// �@���@���g�J
	for (auto face_it = m_copied_mesh->faces_sbegin(); face_it != m_copied_mesh->faces_end(); ++face_it) {
		int face_origin = COPIED_face_origin[*face_it];

		// �̧Ǽg�J���W���T���I��texcoord
		int id = 0;
		auto fv_it = m_copied_mesh->cfv_ccwbegin(*face_it);
		for (; fv_it.is_valid(); ++fv_it) {
			auto texcoord = m_copied_mesh->texcoord2D(*fv_it);
			vertices_texcoord.emplace_back(texcoord[0], texcoord[1]);
			ORIGIN_texcoord_vbo[face_origin * 3 + id] = glm::vec3(texcoord[0], texcoord[1], layer);
			++id;
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	// �Nvertices_texcoord�s�itexture manager�ۤv��vbo
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices_texcoord.size() * sizeof(glm::vec2), vertices_texcoord.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void CG::TextureManager::RenderResult()
{
	if (m_copied_mesh.has_value()) {
		// clear part of the screen
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, 400, 200);
		glClearColor(1, 1, 1, 1); // white
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(m_vao);
		glUseProgram(m_program);

		// Rendering
		glUniform1i(glGetUniformLocation(m_program, "is_wireframe"), false);
		glViewport(0, 0, 200, 200);
		glDrawArrays(GL_TRIANGLES, 0, m_copied_mesh->n_faces() * 3);

		// Render Wireframe
		glUniform1i(glGetUniformLocation(m_program, "is_wireframe"), true);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glViewport(200, 0, 200, 200);
		glDrawArrays(GL_TRIANGLES, 0, m_copied_mesh->n_faces() * 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUseProgram(0);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}
}
