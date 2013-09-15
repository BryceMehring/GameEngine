#ifndef _PROGRESSBAR_
#define _PROGRESSBAR_

#include "IElement.h"
#include "IKMInput.h"
#include "IRenderer.h"
#include "Delegates.h"
#include <glm/glm.hpp>

namespace UI
{

// todo: make this a template?
// A ProgressBar displays the progress from a percentage as a colored line
class ProgressBar : public IElement
{
public:

	typedef Delegate<void,void> DELEGATE;

	// ProgressBar ctor
	// start: starting point of ProgressBar
	// end: ending point of ProgressBar
	// callback: gets called whenever the progress bar reaches 1.0
	ProgressBar(const glm::vec2& start, const glm::vec2& end, const DELEGATE& callback);

	virtual void Select();
	virtual void Deselect();
	virtual void Trigger();

	virtual void Update(IKMInput&, double dt);
	virtual void Render(IRenderer&);

	// sets the current progress of the bar, value must be between 0 and 1, if not, the value is clamped into this range
	void SetProgress(float);

private:

	glm::vec2 m_pos;
	glm::vec2 m_start;
	glm::vec2 m_end;
	DELEGATE m_callback;

};

}

#endif // _PROGRESSBAR_