#include "ButtonBase.h"
#include "IRenderer.h"
#include "IInput.h"

#include <glm/gtx/transform.hpp>

namespace UI
{
	

ButtonBase::ButtonBase(const Math::FRECT& s, const glm::vec3& defaultColor,
					   const glm::vec3& hoverColor, float scale, const std::string& str) :
					   m_sprite(s), m_defaultColor(defaultColor), m_hoverColor(hoverColor), m_scale(scale),
					   m_text(str),  m_bMouseHover(false), m_bSelected(false) {}

void ButtonBase::Update(::IInput& input)
{
	// todo: I could optimize this by checking first if the mouse is moving
	// then move on to check if the mouse is colliding with the button
	if(m_sprite.IsPointWithin(input.GetCursorPos()))
	{
		m_bMouseHover = true;
	}
	else
	{
		m_bMouseHover = false;
	}
}

void ButtonBase::Select()
{
	m_bSelected = true;
}

void ButtonBase::Deselect()
{
	m_bSelected = false;
}

void ButtonBase::Render(IRenderer& renderer)
{
	glm::vec4 color = glm::vec4((m_bMouseHover || m_bSelected) ? m_hoverColor : m_defaultColor,1.0f);

	glm::vec2 pos = m_sprite.Middle();
	glm::mat4 T = glm::translate(pos.x,pos.y,-1.0f);
	T = glm::scale(T,m_sprite.Width(),m_sprite.Height(),1.0f);

	renderer.DrawSprite("button",T);
	renderer.DrawString(m_text.c_str(),glm::vec3(pos.x - (m_sprite.Width() / 2.0f),pos.y,0.0f),m_scale,color);
}

}
