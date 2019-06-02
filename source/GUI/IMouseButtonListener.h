#pragma once

namespace UI {
	class IMouseButtonListener {
	public:
		virtual ~IMouseButtonListener() {}

		virtual void mouseButtonCallback(int, bool) = 0;
	};
}