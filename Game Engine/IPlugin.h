#ifndef _IPLUGIN_
#define _IPLUGIN_

enum DLLType
{
	Rendering,
	Input,
	// more dll types here
};

class __declspec(novtable) IPlugin
{
public:
	

	/*
		Base Plug-in class. 
	*/

	friend class PluginManager;

	// returns the name of the plug-in
	virtual DLLType GetType() const = 0;
 
	//virtual const VersionInfo& GetVersion() = 0;

	// displays a info box about the plug-in
	virtual void About() = 0;

protected:

	//IPlugin() {}
	virtual ~IPlugin() {}
	
};

#endif // _IPLUGIN_