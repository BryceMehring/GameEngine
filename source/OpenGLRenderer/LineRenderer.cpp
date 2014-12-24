#include "LineRenderer.h"
#include "ApplyShader.h"
#include "VertexStructures.h"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>


LineRenderer::LineRenderer(const glm::vec3* pArray, unsigned int uiLength, float fWidth, const glm::vec4& color, const glm::mat4& T)
: m_line(pArray, pArray + uiLength), m_width(fWidth), m_color(color), m_transformation(T)
{
}

void LineRenderer::Render(ApplyShader& shader, const IResource* resource)
{
	shader->SetColor(m_color);

	for (unsigned int i = 1; i < m_line.size(); ++i)
	{
		glm::vec3 diff = m_line[i] - m_line[i - 1];
		float length = glm::length(diff);

		if (length != 0.0f)
		{
			float angle = atan2(diff.y, diff.x);

			glm::mat4 transformation = glm::translate((m_line[i] + m_line[i - 1]) / 2.0f);
			transformation = glm::rotate(transformation, angle, glm::vec3(0.0f, 0.0f, 1.0f));
			transformation = glm::scale(transformation, glm::vec3(length, m_width, 1.0f));

			shader->SetValue("transformation", transformation);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
		}
	}
}
