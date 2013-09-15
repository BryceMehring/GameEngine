#ifndef _IUIELEMENT_
#define _IUIELEMENT_

#include "RefCounting.h"
#include "IKMInput.h"
#include "IRender.h"

namespace UI
{

class IElement : public IRender, public RefCounting
{
public:

	virtual void Select() = 0;
	virtual void Deselect() = 0;
	virtual void Trigger() = 0;

	virtual void Update(IKMInput&, double dt) = 0;

protected:

	virtual ~IElement() {}

};

}

#endif // _IUIELEMENT_