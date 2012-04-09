#ifndef _IINPUT_
#define _IINPUT_

#include "IPlugin.h"

class __declspec(novtable) IInputPlugin : public IPlugin
{
public:

	virtual void Reset() = 0;
	
protected:

	virtual void Poll(const struct MsgProcData&) = 0;

	virtual ~IInputPlugin() {}

};

#endif // _IINPUT_