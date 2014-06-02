#include "Button.h"
#include "IRenderer.h"
#include "IInput.h"

namespace UI
{
	Button::Button(const Math::FRECT& s, const glm::vec3& defaultColor,
		const glm::vec3& hoverColor, float scale, const std::string& str, const DELEGATE& callback) :
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
		glm::vec4 color = glm::vec4((m_bMouseHover || m_bSelected) ? m_hoverColor : m_defaultColor, 1.0f);

		renderer.DrawString(m_text.c_str(), glm::vec3(m_sprite.topLeft, 0.0f), m_scale, color);
		renderer.DrawSprite("button", m_sprite.Transformation());
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
		m_callback();
	}

	void Button::SetCallback(const DELEGATE& callback)
	{
		m_callback = callback;
	}

	void Button::SetText(std::string str)
	{
		m_text = str;
	}

}
