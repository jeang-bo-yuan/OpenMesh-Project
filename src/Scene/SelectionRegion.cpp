#include "SelectionRegion.h"
#include <Utilty/LoadShaders.h>
#include <glm/gtc/type_ptr.hpp>

SelectionRegion::SelectionRegion()
	: m_region_program(0), m_region_start(0, 0)
{
	ShaderInfo shader_region[] = {
		{ GL_VERTEX_SHADER, "./res/shaders/region.vert" },
		{ GL_FRAGMENT_SHADER, "./res/shaders/region.frag" },
		{ GL_NONE, NULL }
	};
	m_region_program = LoadShaders(shader_region);
}

void SelectionRegion::SetViewport(int w, int h)
{
	glUseProgram(m_region_program);
	glUniform2f(glGetUniformLocation(m_region_program, "viewport"), w, h);
	glUseProgram(0);
}

void SelectionRegion::Render(glm::vec2 RegionEnd)
{
	glUseProgram(m_region_program);
	glUniform2fv(glGetUniformLocation(m_region_program, "regionStart"), 1, glm::value_ptr(m_region_start));
	glUniform2fv(glGetUniformLocation(m_region_program, "regionEnd"), 1, glm::value_ptr(RegionEnd));
	glUniform1i(glGetUniformLocation(m_region_program, "selectFace"), GL_FALSE);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glUseProgram(0);
}

void SelectionRegion::RenderAndSelect(glm::vec2 RegionEnd, GLuint IDtexture, GLuint selectedSSBO, int output)
{
	glUseProgram(m_region_program);
	glUniform2fv(glGetUniformLocation(m_region_program, "regionStart"), 1, glm::value_ptr(m_region_start));
	glUniform2fv(glGetUniformLocation(m_region_program, "regionEnd"), 1, glm::value_ptr(RegionEnd));
	glUniform1i(glGetUniformLocation(m_region_program, "selectFace"), GL_TRUE);
	glUniform1i(glGetUniformLocation(m_region_program, "SSBOoutput"), output);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, IDtexture);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, selectedSSBO);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// make sure data is written into SSBO
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	// unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glUseProgram(0);
}
