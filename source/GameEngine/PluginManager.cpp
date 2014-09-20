// Programmed By Bryce Mehring

#include "PluginManager.h"
#include "Log.h"

#include <algorithm>
#include <sstream>
#include <iostream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0)
        return "No error message has been recorded";

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}
#else
#include <dlfcn.h>
#endif // _WIN32

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
	delete pPlugin;

	if(mod != nullptr)
	{
#ifdef _WIN32
		FreeLibrary((HMODULE)mod);
#else
		dlclose(mod);
#endif
	}
}

PluginManager::PluginManager()
{
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

	std::shared_ptr<PluginInfo> dll = std::make_shared<PluginInfo>();

	std::string dllFile = folder + '/' + file + ".plug";

#if defined(_WIN32)
	dll->mod = LoadLibrary(dllFile.c_str());
#else
	dll->mod = dlopen(dllFile.c_str(),RTLD_NOW);
#endif

	if(dll->mod == nullptr)
	{
		std::string error;
#ifdef _WIN32
		error = GetLastErrorAsString();
#else
		error = dlerror();
#endif
		throw error;
	}

	CREATEPLUGIN pFunct = nullptr;

#ifdef _WIN32
	pFunct = (CREATEPLUGIN)GetProcAddress((HMODULE)(dll->mod),"CreatePlugin");
#else
	pFunct = (CREATEPLUGIN)dlsym(dll->mod,"CreatePlugin");
#endif

	if(pFunct == nullptr)
	{
		throw ("Corrupted Shared Library: "  + file);
	}

	// Create the plugin
	dll->pPlugin = pFunct();

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
		m_plugins.insert({ type, dll });
	}

	// return the plugin interface
	return dll->pPlugin;
}

void PluginManager::FreePlugin(DLLType type)
{
	m_plugins.erase(type);
}

void PluginManager::FreeAllPlugins()
{
	m_plugins.clear();
}

