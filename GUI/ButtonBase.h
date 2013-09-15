#ifndef _BUTTONBASE_
#define _BUTTONBASE_

#include "IElement.h"
#include "IKMInput.h"
#include "IRenderer.h"
#include "VecMath.h"
#include <glm/glm.hpp>
#include <string>

namespace UI
{

class ButtonBase : public IElement
{
public:

	ButtonBase(const Math::FRECT& s, const glm::vec3& defaultColor,
			   const glm::vec3& hoverColor, const glm::vec2& scale, const std::string& str);

	virtual void Update(::IKMInput&);
	virtual void Render(::IRenderer&);

	virtual void Select();
	virtual void Deselect();

protected:

	virtual ~ButtonBase() {}

	Math::FRECT m_sprite;

private:
	glm::vec3 m_defaultColor;
	glm::vec3 m_hoverColor;
	glm::vec2 m_scale;

	std::string m_text;

	bool m_bMouseHover;
	bool m_bSelected;

};

}

#endif // _BUTTONBASE_