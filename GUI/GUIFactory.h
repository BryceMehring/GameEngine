#ifndef _GUIFACTORY_
#define _GUIFACTORY_

#include "IElement.h"
#include "Button.h"
#include "Game.h"

namespace UI
{
	template< class T >
	class GUIFactory
	{
	public:

		template< class ...A >
		static IElement* CreateElement(Game& theGame, const A&... args)
		{
			return new T(args...);
		}

		template< class ...A >
		static IElement* CreateElement(Game& theGame, A&&... args)
		{
			return new T(std::forward<A>(args)...);
		}
	};

	template<>
	class GUIFactory<Button>
	{
	public:

		static IElement* CreateElement(Game& theGame,
									   const glm::vec2& pos,
									   const glm::vec3& defaultColor,
									   const glm::vec3& hoverColor,
									   float textScale,
									   const std::string& str,
									   const Button::DELEGATE& callback)
		{
			Math::FRECT R(pos);
			theGame.GetRenderer().GetStringRect(str.c_str(), textScale, FontAlignment::Center, R);

			return new Button(R, defaultColor, hoverColor, textScale, str, callback);
		}
	};

}

#endif // _GUIFACTORY_
