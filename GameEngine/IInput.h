#ifndef _IINPUT_
#define _IINPUT_

#include "IPlugin.h"

class IInputPlugin : public IPlugin
{
public:

	// reset all once only data members
	virtual void Reset() = 0;
	
protected:

	virtual ~IInputPlugin() {}

};

#endif // _IINPUT_
