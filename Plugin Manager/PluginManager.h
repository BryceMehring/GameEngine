// Programmed by Bryce Mehring 
// 1/12/2011

#pragma once
#include <Windows.h>
#include <vector>
#include "Singleton.h"
#include "Interfaces.h"

// Macros
#define PLUGINDECL __declspec(dllexport)

class PluginManager;
typedef IPlugin* (*CREATEPLUGIN)(PluginManager* mgr);


struct BasicDLL
{
	HMODULE mod;
	CREATEPLUGIN func;
};


class PluginManager : public RefCounting
{
public:

	/* 
	   This class manages all of the dll plugins. It will load and unload them when needed.
	   This class is also a singleton because it needs global access.
	*/

	PluginManager();
	virtual ~PluginManager();

	HINSTANCE GetHINSTANCE() const;
	HWND GetWindowHandle() const;

	IPlugin* LoadDLL(char* pDLL);
	//void LoadAllPlugins(char* pDictionary);


private:

	std::vector<BasicDLL> m_dlls;
	
};