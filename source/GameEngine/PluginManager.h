// Programmed by Bryce Mehring

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
#include <string>

/*
   The PluginManager manages all plugins. It will load and unload them when needed.
*/
class PluginManager
{
public:

	PluginManager();
	PluginManager(const PluginManager&) = delete;
	PluginManager& operator=(const PluginManager&) = delete;

	// returns the plugin if found, else, it returns nullptr
	const IPlugin* GetPlugin(DLLType type) const;
	IPlugin* GetPlugin(DLLType type);
	
	/* Plugin must have the extension ".plug".
	   Returns a valid plugin if loaded.
	   In the case of error, a string is thrown with the error message.
	   Note: Each plugin must export: "IPlugin* CreatePlugin()" used to create the plugin.
	 */
	IPlugin* LoadPlugin(std::string file);

	// Frees the plugin given
	// todo: need to return if this function succeeded or not
	void FreePlugin(DLLType type);

	// removes all plugins loaded
	void FreeAllPlugins();

private:

	typedef std::map<DLLType,std::shared_ptr<struct PluginInfo>> plugin_type;
	plugin_type m_plugins; // the list of all plugins

	// Returns the filename without the path to the file
	std::string StripFile(std::string file) const;
};

typedef IPlugin* (*CREATEPLUGIN)();

#endif // _PLUGIN_MANAGER_
