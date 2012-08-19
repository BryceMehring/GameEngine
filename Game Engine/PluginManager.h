// Programmed by Bryce Mehring 
// 1/12/2011

#ifndef _PLUGIN_MANAGER_
#define _PLUGIN_MANAGER_
#pragma once

#ifdef PLUGIN_EXPORTS
#define PLUGINDECL __declspec(dllexport)
#else
#define PLUGINDECL __declspec(dllimport)
#endif

#include "IPlugin.h"
#include "Delegates.h"
#include <hash_map>
#include <Windows.h>

struct PluginInfo
{
	IPlugin* pPlugin;
	HMODULE mod;
};

class PluginManager
{
public:

	/* 
	   This class manages all of the dll plugins. It will load and unload them when needed.
	*/

	PluginManager();
	~PluginManager();

	// more functions would go here as needed to work with the dlls...
	//  ===== interface with dlls =====
	class WindowManager& GetWindowManager();
	class asVM& GetAngelScript();
	//asVM& GetScriptVM() const;

	// Returns all of the keys for all of the dll's that are loaded
	const std::vector<DLLType>& GetPluginKeys() const { return m_Keys; }

	// returns the plugin if found, else, it returns null
	const IPlugin* GetPlugin(DLLType type) const;
	IPlugin* GetPlugin(DLLType type);
	IPlugin* LoadDLL(const char* pFile);

	bool LoadAllPlugins(const std::string& path, const std::string& ext);
	
	void FreePlugin(DLLType type);
	void FreeAllPlugins();

	// todo: is this good to do?
	void SetGame(class Game* pGame) { m_pGame = pGame; }

	// todo: need to implement this?
	virtual void RegisterScript() {}

private:

	class Game* m_pGame;
	typedef stdext::hash_map<DLLType,PluginInfo> plugin_type;
	plugin_type m_plugins;

	std::vector<DLLType> m_Keys;

	// Helper functions
	void FreePlugin(const PluginInfo& plugin);

	// Prevent copying
	PluginManager(const PluginManager&);
	PluginManager& operator=(const PluginManager&);
};

typedef IPlugin* (*CREATEPLUGIN)(PluginManager& mgr);

extern "C" PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr);

#endif // _PLUGIN_MANAGER_