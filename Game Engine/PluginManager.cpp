// Programmed By Bryce Mehring
// 1/20/2011

// Read chapter 16, use the dynamic object mapper with the DLL files.
#include "StdAfx.h"
#include "PluginManager.h"

#pragma comment(lib,"Game Engine.lib")

IMPL_SINGLETON(PluginManager);

PluginManager::PluginManager() : m_pEngine(g_pEngine)
{
	m_pEngine->AddRef();
	m_plugins.reserve(10);
}

PluginManager::~PluginManager()
{
	for(std::vector<PluginInfo>::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		delete iter->pPlugin;
		FreeLibrary(iter->mod);
	}
	m_pEngine->Release();
}

HINSTANCE PluginManager::GetHINSTANCE() const
{
	return m_pEngine->GetHINSTANCE();
}

HWND PluginManager::GetWindowHandle() const
{
	return m_pEngine->GetWindowHandle();
}

IPlugin* PluginManager::GetPlugin(unsigned int index)
{
	return (index >= 0) ? m_plugins[index].pPlugin : NULL;
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