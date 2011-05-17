// Programmed By Bryce Mehring
// 1/20/2011

// Read chapter 16, use the dynamic object mapper with the DLL files.
#include "StdAfx.h"
#include "PluginManager.h"
#include "asVM.h"

#pragma comment(lib,"Game Engine.lib")

using namespace AngelScript;

PluginManager::PluginManager() : m_pEngine(IBaseEngine::Instance())
{
}

// ===== Destructor =====
PluginManager::~PluginManager()
{
	for(plugin_type::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		delete iter->second.pPlugin;
		FreeLibrary(iter->second.mod);
	}
	m_pEngine = NULL;
}

//  ===== interface with dlls =====
HINSTANCE PluginManager::GetHINSTANCE() const
{
	return m_pEngine->GetHINSTANCE();
}

HWND PluginManager::GetWindowHandle() const
{
	return m_pEngine->GetWindowHandle();
}

IPlugin* PluginManager::GetPlugin(DLLType type) const
{
	IPlugin* pPlugin = NULL;
	plugin_type::const_iterator iter = m_plugins.find(type);

	if(iter != m_plugins.end())
	{
		pPlugin = iter->second.pPlugin;
	}

	return pPlugin;
}

IPlugin* PluginManager::LoadDLL(const char* pDLL)
{
	PluginInfo dll;
	dll.mod = LoadLibrary(pDLL);

	if(!dll.mod)
	{
		//error?
	}

	CREATEPLUGIN pFunct = (CREATEPLUGIN)GetProcAddress(dll.mod,"CreatePlugin");
	dll.pPlugin = pFunct(*this);

	DLLType type = dll.pPlugin->GetType();

	m_plugins.insert(make_pair(type,dll));

	return dll.pPlugin;
}

void PluginManager::RegisterScript()
{
	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();

	DBAS(pEngine->RegisterObjectType("PluginManager",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterGlobalProperty("PluginManager pm",this));
}