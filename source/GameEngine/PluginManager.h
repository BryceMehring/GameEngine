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
#include <memory>

/*
   The PluginManager manages all of the dll plugins. It will load and unload them when needed.
*/
class PluginManager
{
public:

	PluginManager();

	// returns the plugin if found, else, it returns nullptr
	const IPlugin* GetPlugin(DLLType type) const;
	IPlugin* GetPlugin(DLLType type);
	
	/* Loads a plugin dll. Plugin dll's have the extension, ".plug".
	   Returns a valid plugin if loaded.
	   In the case of error, a string is thrown with the error message.
	   Note: Each plugin must export: "IPlugin* CreatePlugin()" used to create the plugin.
	 */
	IPlugin* LoadDLL(std::string file, std::string folder = "./");

	// Frees the plugin given
	// todo: need to return if this function succeeded or not
	void FreePlugin(DLLType type);

	// removes all plugins loaded
	void FreeAllPlugins();

private:

	typedef std::map<DLLType,std::shared_ptr<struct PluginInfo>> plugin_type;
	plugin_type m_plugins; // the list of all plugins

	// Prevent copying
	PluginManager(const PluginManager&) = delete;
	PluginManager& operator=(const PluginManager&) = delete;
};

typedef IPlugin* (*CREATEPLUGIN)();

#endif // _PLUGIN_MANAGER_
