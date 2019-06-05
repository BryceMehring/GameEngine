#include "Slider.h"
#include <glm/gtx/transform.hpp>
#include <sstream>

namespace UI
{
	Slider::Slider(const glm::vec2& start, const glm::vec2& end,
	float min, float max, int precision, const std::string& tex, const std::string& text, const DELEGATE& callback) :
	m_SpriteTexture(tex), m_text(text), m_callback(callback), m_pos(start), m_start(start), m_end(end),
	m_fPercentage(min), m_fMin(min), m_fMax(max), m_iPrecision(precision), m_bEnable(false), m_bUpdateEnable(false)
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

	void Slider::mouseButtonCallback(int button, bool pressed)
	{
		m_clicked = button == 0 && pressed;
	}

	void Slider::mousePosCallback(glm::dvec2 cursorPos, glm::dvec2 windowCursorPos, glm::dvec2 acceleration)
	{
		if (m_clicked) {
			Math::FRECT R((m_end.x - m_start.x), 50.0f, (m_end + m_start) * 0.5f);
			if (R.IsPointWithin(windowCursorPos))
			{
				// Move the slider to the mouse pos
				m_pos.x = glm::clamp(static_cast<float>(windowCursorPos.x), m_start.x, m_end.x);

				Trigger();
			}
		}
	}

	void Slider::Render(IRenderer& renderer)
	{
		glm::vec3 line[2] =
		{
			glm::vec3(m_start.x,m_start.y,0.0f),
			glm::vec3(m_end.x,m_end.y,0.0f)
		};

		glm::mat4 T = glm::translate(glm::vec3(m_pos.x,m_pos.y,0.0f));
		T = glm::scale(T,glm::vec3(60.0f,40.0f,1.0f));

		std::stringstream stream;
		stream << m_text << ": ";
		stream.precision(m_iPrecision);
		stream << std::fixed << m_fPercentage << std::endl;

		renderer.SetRenderSpace(RenderSpace::Screen);

		renderer.DrawSprite(m_SpriteTexture,T);
		renderer.DrawLine(line,2);
		renderer.DrawString(stream.str().c_str(),glm::vec3(m_start.x,m_end.y - 10.0f,0.0f));

	}

	float Slider::GetValue() const
	{
		return m_fPercentage;
	}

	void Slider::SetValue(float v)
	{
		float sliderLength = abs(m_end.x - m_start.x);
		float p = (v - m_fMin) / (m_fMax - m_fMin);

		m_pos.x = p * sliderLength + m_start.x;

		Trigger();
	}

}
