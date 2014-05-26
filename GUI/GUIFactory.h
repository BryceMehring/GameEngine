#ifndef _GUIFACTORY_
#define _GUIFACTORY_

#include "IElement.h"
#include "Button.h"
#include "Game.h"

#include <memory>

namespace UI
{
	template< class T >
	class GUIFactory
	{
	public:

		template< class ...A >
		static std::shared_ptr<IElement> CreateElement(Game& theGame, const A&... args)
		{
			return std::make_shared<T>(args...);
		}

		template< class ...A >
		static std::shared_ptr<IElement> CreateElement(Game&, A&&... args)
		{
			return std::make_shared<T>(std::forward<A>(args)...);
		}
	};

	template<>
	class GUIFactory<Button>
	{
	public:

		static std::shared_ptr<IElement> CreateElement(Game& theGame,
									   const glm::vec2& pos,
									   const glm::vec3& defaultColor,
									   const glm::vec3& hoverColor,
									   float textScale,
									   const std::string& str,
									   const Button::DELEGATE& callback)
		{
			Math::FRECT R(pos);
			theGame.GetRenderer().GetStringRect(str.c_str(), textScale, FontAlignment::Center, R);

			return std::make_shared<Button>(R, defaultColor, hoverColor, textScale, str, callback);
		}
	};

}

#endif // _GUIFACTORY_
