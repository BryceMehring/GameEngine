#include "Slider.h"
#include <glm/gtx/transform.hpp>
#include <sstream>

namespace UI
{
Slider::Slider(const glm::vec2& start,
			   const glm::vec2& end,
			   float min, float max, const std::string& tex, const DELEGATE& callback) :
			   m_pos(start), m_start(start), m_end(end), m_fPercentage(min), m_fMin(min),
			   m_fMax(max), m_bEnable(false), m_bUpdateEnable(false), m_callback(callback), m_SpriteTexture(tex)
{
}

void Slider::Select()
{
	m_bEnable = true;
}

void Slider::Deselect()
{
	m_bEnable = false;
}

void Slider::Trigger()
{
	// todo: this could be moved somewhere else
	float sliderLength = abs(m_end.x - m_start.x);
	float posLength = abs(m_start.x - m_pos.x);

	m_fPercentage = (m_fMax - m_fMin) * posLength / sliderLength + m_fMin;
	m_callback(m_fPercentage);
}

void Slider::Update(IInput& input, double dt)
{
	if(input.MouseClick(0))
	{
		Math::FRECT R((m_end.x - m_start.x),50.0f,(m_end + m_start) * 0.5f);
		if(R.IsPointWithin(input.GetCursorPos()))
		{
			m_bUpdateEnable = true;
		}
	}
	else if(input.MouseRelease(0))
	{
		m_bUpdateEnable = false;
	}

	if(m_bUpdateEnable)
	{
		// Move the slider to the mouse pos

		const glm::vec2& mousePos = input.GetCursorPos();
		m_pos.x = glm::clamp(mousePos.x,m_start.x,m_end.x);

		Trigger();
	}
}

void Slider::Render(IRenderer& renderer)
{
	glm::vec3 line[2] =
	{
		glm::vec3(m_start.x,m_start.y,0.0f),
		glm::vec3(m_end.x,m_end.y,0.0f)
	};

	glm::mat4 T = glm::translate(glm::vec3(m_pos.x,m_pos.y,1.0f));
	T = glm::scale(T,glm::vec3(60.0f,40.0f,1.0f));

	std::stringstream stream;
	stream.precision(2);
	stream << std::fixed << m_fPercentage << std::endl;

	renderer.SetRenderSpace(RenderSpace::Screen);

	renderer.DrawSprite(m_SpriteTexture,T);
	renderer.DrawLine(line,2);
	renderer.DrawString(stream.str().c_str(),glm::vec3((m_end.x + m_start.x) / 2.0f - 10.0f,m_end.y - 10.0f,0.0f));

}

float Slider::GetValue() const
{
	return m_fPercentage;
}

}
