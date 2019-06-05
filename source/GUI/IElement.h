#ifndef _IUIELEMENT_
#define _IUIELEMENT_

#include "IInput.h"
#include "IRender.h"
#include "IMouseButtonListener.h"
#include "IMousePosListener.h"

namespace UI
{

class IElement : public IRender, public IMouseButtonListener, public IMousePosListener
{
public:

	virtual ~IElement() {}

	virtual void Select() = 0;
	virtual void Deselect() = 0;
	virtual void Trigger() = 0;

};

}

#endif // _IUIELEMENT_
