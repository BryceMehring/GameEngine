// Programmed By Bryce Mehring
// 1/20/2011

// Read chapter 16, use the dynamic object mapper with the DLL files.
#include "PluginManager.h"

IMPL_SINGLETON(PluginManager);


PluginManager::PluginManager() : m_engine(*(BEngine::GetInstance()))
{
	m_plugins.reserve(10);
}

PluginManager::~PluginManager()
{
	for(std::vector<PluginInfo>::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		delete iter->pPlugin;
		FreeLibrary(iter->mod);
	}
}

HINSTANCE PluginManager::GetHINSTANCE() const
{
	return m_engine.GetHINSTANCE();
}

HWND PluginManager::GetWindowHandle() const
{
	return m_engine.GetWindowHandle();
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
	dll.pPlugin = pFunct(*this);

	m_plugins.push_back(dll);

	return dll.pPlugin;
}