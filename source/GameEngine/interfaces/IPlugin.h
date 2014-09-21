#ifndef _IPLUGIN_
#define _IPLUGIN_

enum class DLLType
{
	Rendering,
	Input,
	Game
};

// Plugin Interface
class IPlugin
{
public:


	/*
		Base Plug-in class.
	*/

	friend struct PluginInfo;

	// returns the type of the plug-in
	virtual DLLType GetPluginType() const = 0;

	// returns the name of the plug-in
	virtual const char* GetName() const = 0;

	// returns the version number of the plugin
	virtual int GetVersion() const = 0;

protected:

	virtual ~IPlugin() {}

};

#endif // _IPLUGIN_
