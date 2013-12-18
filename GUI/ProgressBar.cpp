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
	if(abs(m_end.x - m_pos.x) < 0.01f || abs(m_start.x - m_pos.x) < 0.01f)
	{
		m_callback.Call();
	}
}
void ProgressBar::Render(IRenderer& renderer)
{
	glm::vec3 progStart[2] =
	{
		glm::vec3(m_start,-100.0f),
		glm::vec3(m_pos,-100.0f)
	};

	glm::vec3 progEnd[2] =
	{
		glm::vec3(m_pos,-100.0f),
		glm::vec3(m_end,-100.0f)
	};

	renderer.SetRenderSpace(World);

	renderer.DrawLine(progStart,2,2.0f,glm::vec4(1.0f,0.0f,0.0f,1.0f));
	renderer.DrawLine(progEnd,2,2.0f);
}

void ProgressBar::SetProgress(float v)
{
	float dist = m_end.x - m_start.x;
	m_pos.x = m_start.x + dist * glm::clamp(v,0.0f,1.0f);
}

void ProgressBar::SetPos(const glm::vec2& pos)
{
	glm::vec2 diff = (m_end - m_start) * 0.5f;
	//glm::vec2 posDiff = (m_pos - m_start) * 0.5f;
	float oldPosLength = glm::length(m_pos.x - m_start.x);
	//float oldPosLength = m_pos.x - m_start.x;

	m_start = pos - diff;
	m_end = pos + diff;
	//m_pos.x = oldPosLength;
	m_pos.x = m_start.x + oldPosLength;
	m_pos.y = m_start.y;

	//m_pos += diff;

	//m_pos.x = m_pos.x + diff.x;
	//m_pos.y = pos.y;

	//m_start.y = m_end.y = pos.y;
}

}
