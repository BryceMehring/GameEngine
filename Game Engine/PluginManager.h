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
	HINSTANCE GetHINSTANCE() const;
	HWND GetWindowHandle() const;
	class WindowManager* GetWindowManager() { return m_pEngine; }
	//asVM& GetScriptVM() const;

	// returns the plugin if found, else, it returns null
	IPlugin* GetPlugin(DLLType type) const;

	IPlugin* LoadDLL(const char* pFile);
	bool LoadAllPlugins(const std::string& path, const std::string& ext);
	
	void FreePlugin(DLLType type);
	void FreeAllPlugins();

	// todo: is this good to do?
	void SetEngine(class WindowManager* pEngine) { m_pEngine = pEngine; }

	// todo: need to implement this?
	virtual void RegisterScript() {}

private:

	class WindowManager* m_pEngine;
	typedef stdext::hash_map<DLLType,PluginInfo> plugin_type;
	plugin_type m_plugins;

	// Prevent copying
	PluginManager(const PluginManager&);
	PluginManager& operator=(const PluginManager&);
};

typedef IPlugin* (*CREATEPLUGIN)(PluginManager& mgr);

extern "C" PLUGINDECL IPlugin* CreatePlugin(PluginManager& mgr);

#endif // _PLUGIN_MANAGER_