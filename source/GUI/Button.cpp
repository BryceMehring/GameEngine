#include "Button.h"
#include "IRenderer.h"
#include "IInput.h"
#include "Game.h"

namespace UI
{
	Button::Button(::Game& game, const glm::vec4& defaultColor,
		const glm::vec4& hoverColor, float scale, const std::string& str, const DELEGATE& callback) :
		m_defaultColor(defaultColor), m_hoverColor(hoverColor), m_scale(scale),
		m_text(str), m_callback(callback), m_bMouseHover(false), m_bSelected(false) {

		game.GetRenderer().GetStringRect(m_text.c_str(), m_scale, FontAlignment::Center, m_sprite);
	}


	void Button::Update(::IInput& input, double)
	{
	}

	void Button::Render(IRenderer& renderer)
	{
		glm::vec4 color = (m_bMouseHover || m_bSelected) ? m_hoverColor : m_defaultColor;

		renderer.DrawString(m_text.c_str(), glm::vec3(m_sprite.topLeft, 0.0f), color, m_sprite.Height());
		renderer.DrawSprite("button", m_sprite.Transformation(), glm::vec4(1.0f - glm::vec3(m_defaultColor), m_defaultColor.a));
	}

	void Button::mouseButtonCallback(int key, bool pressed)
	{
		if (m_bMouseHover && key == 0 && pressed)
		{
			Trigger();
		}
	}

	void UI::Button::mousePosCallback(glm::dvec2 cursorPos, glm::dvec2 windowCursorPos, glm::dvec2 acceleration)
	{
		m_bMouseHover = m_sprite.IsPointWithin(windowCursorPos);
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
