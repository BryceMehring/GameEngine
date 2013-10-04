#include "ProgressBar.h"

namespace UI
{

ProgressBar::ProgressBar(const glm::vec2& start, const glm::vec2& end, const DELEGATE& callback) : m_pos(start),
	m_start(start), m_end(end), m_callback(callback)
{
}

void ProgressBar::Select()
{
}
void ProgressBar::Deselect()
{
}
void ProgressBar::Trigger()
{
}

void ProgressBar::Update(class IKMInput&, double dt)
{
	if((m_end.x - m_pos.x) < 0.01f)
	{
		m_callback.Call();
	}
}
void ProgressBar::Render(IRenderer& renderer)
{
	glm::vec3 progStart[2] =
	{
		glm::vec3(m_start,0.0f),
		glm::vec3(m_pos,0.0f)
	};

	glm::vec3 progEnd[2] =
	{
		glm::vec3(m_pos,0.0f),
		glm::vec3(m_end,0.0f)
	};

	float min, max;
	renderer.GetLineWidthRange(min, max);

	renderer.DrawLine(progStart,2,max,glm::vec4(1.0f,0.0f,0.0f,1.0f));
	renderer.DrawLine(progEnd,2,max);
}

void ProgressBar::SetProgress(float v)
{
	float dist = m_end.x - m_start.x;
	m_pos.x = m_start.x + dist * glm::clamp(v,0.0f,1.0f);
}

}
