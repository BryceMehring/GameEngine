// Programmed By Bryce Mehring

// Read chapter 16, use the dynamic object mapper with the DLL files.
#include "PluginManager.h"
#include "Log.h"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <angelscript.h>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace std;

struct PluginInfo
{
	PluginInfo();
	~PluginInfo();

	IPlugin* pPlugin;
	void* mod;
};

PluginInfo::PluginInfo() : pPlugin(nullptr), mod(nullptr)
{
}

PluginInfo::~PluginInfo()
{
	if(pPlugin != nullptr)
	{
		delete pPlugin;
	}

	if(mod != nullptr)
	{
#ifdef _WIN32
	FreeLibrary((HMODULE)mod);
#else
	dlclose(mod);
#endif
	}
}

PluginManager::PluginManager() : m_pAS(nullptr)
{
}

// ===== Destructor =====
PluginManager::~PluginManager()
{
	FreeAllPlugins();
	m_pAS->Release();
}

void PluginManager::FreeAllPlugins()
{
	for(auto iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		FreePlugin(*(iter->second));
	}

	m_plugins.clear();
}

const IPlugin* PluginManager::GetPlugin(DLLType type) const
{
	plugin_type::const_iterator iter = m_plugins.find(type);

	if(iter != m_plugins.end())
	{
		return iter->second->pPlugin;
	}

	return nullptr;
}

IPlugin* PluginManager::GetPlugin(DLLType type)
{
	plugin_type::const_iterator iter = m_plugins.find(type);

	if(iter != m_plugins.end())
	{
		return iter->second->pPlugin;
	}

	return nullptr;
}

IPlugin* PluginManager::LoadDLL(std::string file, std::string folder)
{
	Log::Instance().Write("Loading " + file);

	shared_ptr<PluginInfo> dll(new PluginInfo);

	std::string dllFile = folder + '/' + file + ".plug";

#if defined(_WIN32)
	dll->mod = LoadLibrary(dllFile.c_str());
#else
	dll->mod = dlopen(dllFile.c_str(),RTLD_NOW);
#endif

	if(dll->mod == nullptr)
	{
		Log::Instance().Write("Cannot open: " + dllFile);
#ifdef _WIN32
		Log::Instance().Write(GetLastError());
#else
		Log::Instance().Write(dlerror());
#endif

		return nullptr;
	}

	CREATEPLUGIN pFunct = nullptr;

#ifdef _WIN32
	pFunct = (CREATEPLUGIN)GetProcAddress((HMODULE)(dll->mod),"CreatePlugin");
#else
	pFunct = (CREATEPLUGIN)dlsym(dll->mod,"CreatePlugin");
#endif

	if(pFunct == nullptr)
	{
		Log::Instance().Write("Corrupted Shared Library: " + file);

		return nullptr;
	}

	// Create the plugin
	dll->pPlugin = pFunct();
	dll->pPlugin->Init(m_pAS);

	// Get the type of the plugin
	DLLType type = dll->pPlugin->GetPluginType();
	auto iter = m_plugins.find(type); // see if the plugin is already loaded

	// If it is already loaded
	if(iter != m_plugins.end())
	{
		// replace with new
		iter->second = dll;
	}
	else
	{
		// insert plugin into map
		m_plugins.insert(make_pair(type,dll));
	}

	// return the plugin interface
	return dll->pPlugin;
}

void PluginManager::FreePlugin(const PluginInfo& plugin)
{
	if(plugin.pPlugin != nullptr)
	{
		plugin.pPlugin->Destroy(m_pAS);
	}
}

void PluginManager::FreePlugin(DLLType type)
{
	auto iter = m_plugins.find(type);
	if(iter != m_plugins.end())
	{
		FreePlugin(*(iter->second));

		m_plugins.erase(iter);
	}
}

