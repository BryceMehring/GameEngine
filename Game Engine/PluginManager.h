// Programmed by Bryce Mehring 
// 1/12/2011

#ifndef _PLUGIN_MANAGER_
#define _PLUGIN_MANAGER_
#pragma once

#include "BEngine.h"
#include "Interfaces.h"

struct PluginInfo
{
	IPlugin* pPlugin;
	HMODULE mod;
};

class PluginManager : public IScripted
{
public:

	/* 
	   This class manages all of the dll plugins. It will load and unload them when needed.
	   This class is also a singleton because it needs global access.
	*/

	PluginManager(IBaseEngine* pEngine);
	virtual ~PluginManager();

	// more functions would go here as needed to work with the dlls...
	//  ===== interface with dlls =====
	HINSTANCE GetHINSTANCE() const;
	HWND GetWindowHandle() const;
	AngelScript::asVM& GetScriptVM() const;

	// returns the plugin if found, else, it returns null
	IPlugin* GetPlugin(DLLType type) const;

	IPlugin* LoadDLL(const char* pFile);
	bool LoadAllPlugins(const std::string& path, const std::string& ext);

	// todo: need to implement this?
	virtual void RegisterScript() {}

private:

	IBaseEngine* m_pEngine;

	typedef std::map<DLLType,PluginInfo> plugin_type;
	plugin_type m_plugins;	
};

#endif // _PLUGIN_MANAGER_