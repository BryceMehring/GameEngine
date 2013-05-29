// Programmed by Bryce Mehring 
// 1/12/2011

#ifndef _PLUGIN_MANAGER_
#define _PLUGIN_MANAGER_

#ifdef _WIN32
#ifdef PLUGIN_EXPORTS
#define PLUGINDECL __declspec(dllexport)
#else
#define PLUGINDECL __declspec(dllimport)
#endif
#else
#define PLUGINDECL
#endif

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
	*/

	PluginManager();
	~PluginManager();

	//  ===== interface with dlls =====

	//bool Good(const char* pFile) const; // todo: need to implement again

	// returns the plugin if found, else, it returns null
	const IPlugin* GetPlugin(DLLType type) const;
	IPlugin* GetPlugin(DLLType type);

	// Loads the dll
	// returns a valid plugin if loaded
	// returns nullptr in the case of error
	// Note: Each plugin must export: "IPlugin* CreatePlugin(asIScriptEngine* as)" used to create the plugin
	// The extension is not needed to be added to the string. On linux, there is no need to prefix the dll's with 'lib'
	IPlugin* LoadDLL(const std::string& dllName);
	
	// Frees the plugin given
	// todo: need to return if this function succeeded or not
	void FreePlugin(DLLType type);

	// removes all plugins loaded
	void FreeAllPlugins();

	// todo: is this good to do?
	void SetAS(class asIScriptEngine* pAS) { m_pAS = pAS; }

	// todo: need to implement this?
	void RegisterScript() {}

private:

	class asIScriptEngine* m_pAS;
	typedef std::map<DLLType,PluginInfo> plugin_type;
	plugin_type m_plugins; // the list of all plugins

	// Helper functions
	void FreePlugin(const PluginInfo& plugin);

	// Prevent copying
	PluginManager(const PluginManager&);
	PluginManager& operator=(const PluginManager&);
};

typedef IPlugin* (*CREATEPLUGIN)();

#endif // _PLUGIN_MANAGER_
