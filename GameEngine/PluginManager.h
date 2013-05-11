// Programmed by Bryce Mehring 
// 1/12/2011

#ifndef _PLUGIN_MANAGER_
#define _PLUGIN_MANAGER_

#define PLUGINDECL
#include <dlfcn.h>

#include "IPlugin.h"
#include <map>
#include <vector>
#include <string>

struct PluginInfo
{
	IPlugin* pPlugin;
	void* mod;
};

class PluginManager
{
public:

	/* 
	   This class manages all of the dll plugins. It will load and unload them when needed.

	   // todo: this class could inherit from a interface which only the plugins would know about, so that the plugins would not
	   // todo: have to link with the game engine lib
	   // note: this would not work
	*/

	PluginManager();
	~PluginManager();

	// more functions would go here as needed to work with the dlls...
	//  ===== interface with dlls =====
	//asVM& GetScriptVM() const;

	//bool Good(const char* pFile) const;

	// returns the plugin if found, else, it returns null
	const IPlugin* GetPlugin(DLLType type) const;
	IPlugin* GetPlugin(DLLType type);
    IPlugin* LoadDLL(const std::string& file);
	
	void FreePlugin(DLLType type);
	void FreeAllPlugins();

	// todo: is this good to do?
	void SetAS(class asIScriptEngine* pAS) { m_pAS = pAS; }

	// todo: need to implement this?
	virtual void RegisterScript() {}

private:

	class asIScriptEngine* m_pAS;
	//typedef 
	typedef std::map<DLLType,PluginInfo> plugin_type;
	plugin_type m_plugins;

	// Helper functions
	void FreePlugin(const PluginInfo& plugin);

	// Prevent copying
	PluginManager(const PluginManager&);
	PluginManager& operator=(const PluginManager&);
};

typedef IPlugin* (*CREATEPLUGIN)(class asIScriptEngine*);

#endif // _PLUGIN_MANAGER_
