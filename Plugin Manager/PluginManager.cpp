// Programmed By Bryce Mehring
// 1/20/2011

// Read chapter 16, use the dynamic object mapper with the DLL files.


#include "PluginManager.h"
#include "BEngine.h"

PluginManager::PluginManager()
{

}

PluginManager::~PluginManager()
{
	for(std::vector<BasicDLL>::iterator iter = m_dlls.begin(); iter != m_dlls.end(); ++iter)
	{
		FreeLibrary(iter->mod);
	}
}

HINSTANCE PluginManager::GetHINSTANCE() const
{
	BEngine* pEngine = BEngine::GetInstance();
	return pEngine->GetHINSTANCE();
}

HWND PluginManager::GetWindowHandle() const
{
	BEngine* pEngine = BEngine::GetInstance();
	return pEngine->GetWindowHandle();
}

IPlugin* PluginManager::LoadDLL(char* pDLL)
{
	BasicDLL dll;
	dll.mod = LoadLibrary(pDLL);
	dll.func = (CREATEPLUGIN)GetProcAddress(dll.mod,"CreatePlugin");

	m_dlls.push_back(dll);

	return m_dlls.back().func(this);
}