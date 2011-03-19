// Programmed by Bryce Mehring 
// 1/12/2011

#pragma once
#include "BEngine.h"
#include "Singleton.h"
#include <vector>

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
	   This class is also a singleton because it needs global access.
	*/

	DECLARE_SINGLETON(PluginManager);

	// more functions would go here as needed to work with the dlls...
	HINSTANCE GetHINSTANCE() const;
	HWND GetWindowHandle() const;

	IPlugin* GetPlugin(unsigned int index);
	IPlugin* LoadDLL(char* pDLL);
	//void LoadAllPlugins(char* pDictionary);


private:

	PluginManager();
	virtual ~PluginManager();

	IBaseEngine* m_pEngine;
	std::vector<PluginInfo> m_plugins;
	
};