#ifndef _IPLUGIN_
#define _IPLUGIN_

enum DLLType
{
	RenderingPlugin,
	InputPlugin,
	GamePlugin
	// more dll types here
};

#define DLLTYPE decltype(DLLType)

class IPlugin
{
public:
	

	/*
		Base Plug-in class.
	*/

	friend class PluginManager;

	// returns the type of the plug-in
	virtual DLLType GetPluginType() const = 0;

	// returns the name of the plug-in
	virtual const char* GetName() const = 0;

	// returns the version number of the plugin
	virtual int GetVersion() const = 0;

	virtual void Init(class asIScriptEngine*) = 0;
	virtual void Destroy(class asIScriptEngine*) = 0;

protected:

	virtual ~IPlugin() {}
	
};

#endif // _IPLUGIN_
