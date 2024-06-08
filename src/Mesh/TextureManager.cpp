#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

CG::TextureManager::TextureManager()
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

//void CG::TextureManager::GenerateTextureCoordinate(MyMesh& the_mesh)
//{
//	// copy selected faces into param
//	OpenMesh::TriMesh_ArrayKernelT<CG::MyTraits> param;
//
//	// do something, then store result int the_mesh
//
//	m_show_parameterization = true;
//}
//
//void CG::TextureManager::RenderResult()
//{
//	if (m_show_parameterization) {
//		// Rendering
//	}
//}
