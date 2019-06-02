#ifndef _BUTTON_
#define _BUTTON_

#include "IElement.h"
#include "IInput.h"
#include "IRenderer.h"
#include "IMouseButtonListener.h"
#include "IMousePosListener.h"
#include "VecMath.h"
#include <glm/glm.hpp>
#include <string>
#include <functional>

class Game;

namespace UI
{
	template< class T >
	class GUIFactory;

	class Button : public IElement, public IMouseButtonListener, public IMousePosListener
	{
	public:

		typedef std::function<void(Button&)> DELEGATE;

		Button(Game& game, const glm::vec4& defaultColor, const glm::vec4& hoverColor, float scale, const std::string& str, const DELEGATE& callback);

		void Update(IInput&, double) override;
		void Render(IRenderer&) override;
		void Destroy(IInput&) override;

		void mouseButtonCallback(int, bool) override;
		void mousePosCallback(glm::dvec2 cursorPos, glm::dvec2 windowCursorPos, glm::dvec2 acceleration) override;

		void Select() override;
		void Deselect() override;
		void Trigger() override;

		void SetCallback(const DELEGATE& callback);

		void SetText(std::string str);
		const std::string& GetText() const;

	protected:

		Math::FRECT m_sprite;

	private:

		glm::vec4 m_defaultColor;
		glm::vec4 m_hoverColor;
		float m_scale;

		std::string m_text;

		DELEGATE m_callback;

		bool m_bMouseHover;
		bool m_bSelected;

		friend class GUIFactory<Button>;
	};

}

#endif // _BUTTON_
