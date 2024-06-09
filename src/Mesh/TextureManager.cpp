#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <OpenMesh/Core/Utils/PropertyManager.hh>

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

	/*glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	ShaderInfo shader[] = {
		{ GL_VERTEX_SHADER, "./res/shaders/texture_plane.vert" },
		{ GL_FRAGMENT_SHADER, "./res/shaders/texture_plane.frag" },
		{ GL_NONE, NULL }
	};
	m_program = LoadShaders(shader);*/
}

bool CG::TextureManager::LoadImage(const std::string& file, int layer)
{
	if (layer < 0 || layer >= MAX_IMG) {
		std::cerr << "Invalid `layer` for TextureManager::LoadImage()\n";
		return false;
	}

	int width = 0, height = 0, channel = 0;
	unsigned char* img = stbi_load(file.c_str(), &width, &height, &channel, 3);

	if (img == nullptr) {
		std::cerr << "Cannot Load Image: " << file << '\n';
		return false;
	}

	if (width != IMG_SIZE || height != IMG_SIZE) {
		std::cerr << "Image Size Should be " << IMG_SIZE << " * " << IMG_SIZE << '\n';
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

	if (layer < 0 || layer >= MAX_IMG) {
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

	auto ORIGIN_mapped_vertex = OpenMesh::VProp<int>(-1, *m_origin_mesh_ptr); // (temporary) 每個點對應到copied mesh中的哪個點
	auto ORIGIN_weight = OpenMesh::EProp<float>(*m_origin_mesh_ptr, "weight"); // origin mesh中每個邊的權重
	auto COPIED_face_origin = OpenMesh::FProp<int>(*m_copied_mesh, "face_origin"); // copied mesh中每個面對應到origin mesh中哪個面
	auto COPIED_weight = OpenMesh::EProp<float>(*m_copied_mesh, "weight"); // copied mesh中每個邊的權重
	m_copied_mesh->request_vertex_texcoords2D();
	
	std::vector<CopiedMesh_t::VertexHandle> new_face_vhandles; // vertex handles of a new copied face

	// for each selected face, copy it
	for (int ORIGIN_face_index = 0; ORIGIN_face_index < m_origin_mesh_ptr->n_faces(); ++ORIGIN_face_index) {
		if (selected_face_arr[ORIGIN_face_index] == false) 
			continue; // not selected, ignore

		new_face_vhandles.clear();

		/* circulate three vertices，複製一份，然後將copied_mesh中的vertex handle存進new_face_vhandles */ {
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
	const CopiedMesh_t::HalfedgeHandle init_heh = first_boundary_heh(*m_copied_mesh);

	// 繞boundary一圈，計算total_length
	for (auto curr = init_heh; curr != init_heh || total_length == 0.f; curr = m_copied_mesh->next_halfedge_handle(curr)) {
#ifndef NDEBUG
		std::cout << m_copied_mesh->from_vertex_handle(curr) << ' ';
#endif
		total_length += m_copied_mesh->calc_edge_length(curr);
	}
#ifndef NDEBUG
	std::cout << "are on boundary" << std::endl;
#endif

	// 設定texCoord（第一個）
	const CopiedMesh_t::VertexHandle init_vh = m_copied_mesh->from_vertex_handle(init_heh);
	m_copied_mesh->set_texcoord2D(init_vh, CG::MyTraits::TexCoord2D(0, 0));
	// 其他的
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

//
//void CG::TextureManager::RenderResult()
//{
//	if (m_show_parameterization) {
//		// Rendering
//	}
//}
