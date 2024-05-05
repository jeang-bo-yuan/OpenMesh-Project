#pragma once
#include <GL/glew.h>
#include <glm/vec2.hpp>

class SelectionRegion
{
	GLuint m_region_program;
	glm::vec2 m_region_start;

public:
	/// Constructor, 載入shader
	SelectionRegion();

	/**
	 * 設定viewport大小
	 * \param w - 寬
	 * \param h - 高
	 */
	void SetViewport(int w, int h);

	/**
	 * 設置範圍的起點
	 * @param newRegionStart - 新的起點（in NDC coordinate）
	 */
	void SetRegionStart(glm::vec2 newRegionStart) { m_region_start = newRegionStart; }

	/**
	 * 繪製範圍
	 * \param RegionEnd - 範圍的終點（in NDC coordinate）
	 */
	void Render(glm::vec2 RegionEnd);

	/**
	 * 繪製 + 選取
	 * \param RegionEnd - 範圍的終點（in NDC coordinate）
	 * \param IDtexture - 從這個texture讀ID（index + 1）
	 * \param selectedSSBO - 若選中則在對應index的位置設成output
	 * \param output - 如果被選中，那麼要在SSBO中存什麼值
	 */
	void RenderAndSelect(glm::vec2 RegionEnd, GLuint IDtexture, GLuint selectedSSBO, int output);
};

