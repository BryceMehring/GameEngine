#pragma once
#include <glm/glm.hpp>

namespace UI {
	class IMousePosListener {
	public:
		virtual ~IMousePosListener() {}

		virtual void mousePosCallback(glm::dvec2 cursorPos, glm::dvec2 windowCursorPos, glm::dvec2 acceleration) = 0;
	};
}