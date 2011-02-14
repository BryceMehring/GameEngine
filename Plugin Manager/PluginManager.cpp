// Programmed By Bryce Mehring
// 1/20/2011

// Read chapter 16, use the dynamic object mapper with the DLL files.
#include "PluginManager.h"


PluginManager::PluginManager() : m_pEngine(g_pEngine)
{
}

PluginManager::~PluginManager()
{
	for(std::vector<PluginInfo>::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		FreeLibrary(iter->mod);
	}
}

HINSTANCE PluginManager::GetHINSTANCE() const
{
	return m_pEngine->GetHINSTANCE();
}

HWND PluginManager::GetWindowHandle() const
{
	return m_pEngine->GetWindowHandle();
}

IPlugin* PluginManager::LoadDLL(char* pDLL)
{
	PluginInfo dll;
	dll.mod = LoadLibrary(pDLL);

	if(!dll.mod)
	{
		//error?
	}

	CREATEPLUGIN pFunct = (CREATEPLUGIN)GetProcAddress(dll.mod,"CreatePlugin");
	dll.pPlugin = pFunct(this);

	m_plugins.push_back(dll);

	return dll.pPlugin;
}