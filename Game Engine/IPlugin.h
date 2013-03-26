#ifndef _IPLUGIN_
#define _IPLUGIN_

enum DLLType
{
	RenderingPlugin,
	InputPlugin,
	GamePlugin,
	// more dll types here
};

#define DLLTYPE decltype(DLLType)

class __declspec(novtable) IPlugin
{
public:
	

	/*
		Base Plug-in class. 
	*/

	friend class PluginManager;

	// returns the name of the plug-in
	virtual DLLType GetPluginType() const = 0;
	virtual const char* GetName() const = 0;
	virtual int GetVersion() const = 0;

protected:

	//IPlugin() {}
	virtual ~IPlugin() {}
	
};

#endif // _IPLUGIN_