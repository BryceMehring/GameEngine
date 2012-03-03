#ifndef _IINPUT_
#define _IINPUT_

#include "IPlugin.h"

class IInputPlugin : public IPlugin
{
public:

	virtual void Poll() = 0;

protected:

	virtual ~IInputPlugin() {}

};

#endif // _IINPUT_