#ifndef _IINPUT_
#define _IINPUT_

#include "IPlugin.h"

// Base Input Plugin
class IInputPlugin : public IPlugin
{
public:

	// processes input events
	virtual void Poll() = 0;
	
protected:

	virtual ~IInputPlugin() {}

};

#endif // _IINPUT_
