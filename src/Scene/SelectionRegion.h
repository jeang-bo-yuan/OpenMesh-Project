#pragma once
#include <GL/glew.h>
#include <glm/vec2.hpp>

class SelectionRegion
{
	GLuint m_region_program;
	glm::vec2 m_region_start;

public:
	/// Constructor, ���Jshader
	SelectionRegion();

	/**
	 * �]�wviewport�j�p
	 * \param w - �e
	 * \param h - ��
	 */
	void SetViewport(int w, int h);

	/**
	 * �]�m�d�򪺰_�I
	 * @param newRegionStart - �s���_�I�]in NDC coordinate�^
	 */
	void SetRegionStart(glm::vec2 newRegionStart) { m_region_start = newRegionStart; }

	/**
	 * ø�s�d��
	 * \param RegionEnd - �d�򪺲��I�]in NDC coordinate�^
	 */
	void Render(glm::vec2 RegionEnd);

	/**
	 * ø�s + ���
	 * \param RegionEnd - �d�򪺲��I�]in NDC coordinate�^
	 * \param IDtexture - �q�o��textureŪID
	 * \param selectedSSBO - �Y�襤�h�b����index�]��true
	 */
	void RenderAndSelect(glm::vec2 RegionEnd, GLuint IDtexture, GLuint selectedSSBO);
};

