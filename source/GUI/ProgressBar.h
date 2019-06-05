#ifndef _PROGRESSBAR_
#define _PROGRESSBAR_

#include "IElement.h"
#include "IInput.h"
#include "IRenderer.h"
#include <glm/glm.hpp>

#include <functional>

namespace UI
{

// todo: make this a template?
// A ProgressBar displays the progress from a percentage as a colored line
class ProgressBar : public IElement
{
public:

	typedef std::function<void(void)> DELEGATE;

	// ProgressBar ctor
	// start: starting point of ProgressBar
	// end: ending point of ProgressBar
	// callback: gets called whenever the progress bar reaches 1.0
	ProgressBar(const glm::vec2& start, const glm::vec2& end, const DELEGATE& callback);

	void Select() override;
	void Deselect() override;
	void Trigger() override;

	void Render(IRenderer&) override;

	void mouseButtonCallback(int, bool) override;
	void mousePosCallback(glm::dvec2 cursorPos, glm::dvec2 windowCursorPos, glm::dvec2 acceleration) override;

	// sets the current progress of the bar, value must be between 0 and 1, if not, the value is clamped into this range
	void SetProgress(float);

	void SetPos(const glm::vec2& pos);

private:

	glm::vec2 m_pos;
	glm::vec2 m_start;
	glm::vec2 m_end;
	DELEGATE m_callback;

};

}

#endif // _PROGRESSBAR_
