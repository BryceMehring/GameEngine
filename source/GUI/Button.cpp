#include "Button.h"
#include "IRenderer.h"
#include "IInput.h"

namespace UI
{
	Button::Button(const Math::FRECT& s, const glm::vec4& defaultColor,
		const glm::vec4& hoverColor, float scale, const std::string& str, const DELEGATE& callback) :
		m_sprite(s), m_defaultColor(defaultColor), m_hoverColor(hoverColor), m_scale(scale),
		m_text(str), m_callback(callback), m_bMouseHover(false), m_bSelected(false) {}

	void Button::Update(::IInput& input, double)
	{
		m_bMouseHover = m_sprite.IsPointWithin(glm::vec2(input.GetCursorPos()));

		if (input.MouseClick(0))
		{
			glm::vec2 pos = glm::vec2(input.GetCursorPos());

			if (m_sprite.IsPointWithin(pos))
			{
				Trigger();
			}
		}
	}

	void Button::Render(IRenderer& renderer)
	{
		glm::vec4 color = (m_bMouseHover || m_bSelected) ? m_hoverColor : m_defaultColor;

		renderer.DrawString(m_text.c_str(), glm::vec3(m_sprite.topLeft, 0.0f), color, m_sprite.Height());
		renderer.DrawSprite("button", m_sprite.Transformation(), glm::vec4(1.0f - glm::vec3(m_defaultColor), m_defaultColor.a));
	}

	void Button::Select()
	{
		m_bSelected = true;
	}

	void Button::Deselect()
	{
		m_bSelected = false;
	}

	void Button::Trigger()
	{
		m_callback(*this);
	}

	void Button::SetCallback(const DELEGATE& callback)
	{
		m_callback = callback;
	}

	void Button::SetText(std::string str)
	{
		m_text = str;
	}

	const std::string& Button::GetText() const
	{
		return m_text;
	}

}
