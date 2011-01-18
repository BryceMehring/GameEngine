// Programmed by Bryce Mehring 
// 1/12/2011

#pragma once
#include "Singleton.h"
#include <Windows.h>

// This class manages all of the dll plugins. It will load and unload them when needed.
// This class is also a singleton because it needs global access.
class PluginManager
{
public:

	DECLARE_SINGLETON(PluginManager);

	HINSTANCE GetHINSTANCE() { return NULL; }
	HWND GetWindowHandle() { return NULL; }
	
};