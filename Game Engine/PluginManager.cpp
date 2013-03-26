// Programmed By Bryce Mehring
// 1/20/2011

#ifdef _WIN32
#include <Windows.h>
#endif

// Read chapter 16, use the dynamic object mapper with the DLL files.
#include "PluginManager.h"
#include "FileManager.h"
#include "Game.h"

#include <algorithm>
#include <cassert>

using namespace std;

PluginManager::PluginManager() : m_pGame(nullptr)
{
}

// ===== Destructor =====
PluginManager::~PluginManager()
{
	FreeAllPlugins();
	this->m_pGame = nullptr;
}

void PluginManager::FreeAllPlugins()
{
	for(auto iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		FreePlugin(iter->second);
	}

	m_Keys.clear();
	m_plugins.clear();
}

asVM& PluginManager::GetAngelScript() { return m_pGame->GetAs(); }

//  ===== interface with dlls =====

const IPlugin* PluginManager::GetPlugin(DLLType type) const
{
	plugin_type::const_iterator iter = m_plugins.find(type);

	if(iter != m_plugins.end())
	{
		return iter->second.pPlugin;
	}

	return nullptr;
}

IPlugin* PluginManager::GetPlugin(DLLType type)
{
	plugin_type::const_iterator iter = m_plugins.find(type);

	if(iter != m_plugins.end())
	{
		return iter->second.pPlugin;
	}

	return nullptr;
}

bool PluginManager::Good(const char* pDLL) const
{
	FileManager& fm = FileManager::Instance();
	char buffer[128];
	PluginInfo dll = {0,0};

	dll.mod = LoadLibrary(pDLL);

	// todo: fix the error handling here
	if(dll.mod == nullptr)
	{
		// error, could not load dll

		// need to output message to log
		// todo: should create a logging singleton that should interact with the console.

		sprintf_s(buffer,"Could not load: %s",pDLL);
		fm.WriteToLog(buffer);

		MessageBox(0,buffer,0,0);
		
		return false;
	}

	CREATEPLUGIN pFunct = (CREATEPLUGIN)GetProcAddress((HMODULE)dll.mod,"CreatePlugin");

	if(pFunct == nullptr)
	{
		sprintf_s(buffer,"CreatePlugin() function not found in: %s",pDLL);
		fm.WriteToLog(buffer);

		MessageBox(0,buffer,0,0);

		return false;
	}

	FreeLibrary((HMODULE)dll.mod);

	return true;
}

IPlugin* PluginManager::LoadDLL(const char* pDLL)
{
	FileManager& fm = FileManager::Instance();
	char buffer[128];
	PluginInfo dll = {0,0};

#ifdef _WIN32
	dll.mod = LoadLibrary(pDLL);
#else
	dll.mod = dlopen(pDLL,RTLD_LAZY);
#endif

	// todo: fix the error handling here
	if(dll.mod == nullptr)
	{
		// error, could not load dll

		// need to output message to log
		// todo: should create a logging singleton that should interact with the console.

		sprintf_s(buffer,"Could not load: %s",pDLL);
		fm.WriteToLog(buffer);

		MessageBox(0,buffer,0,0);
		
		return nullptr;
	}

	CREATEPLUGIN pFunct = nullptr;

#ifdef _WIN32
	pFunct = (CREATEPLUGIN)GetProcAddress((HMODULE)(dll.mod),"CreatePlugin");
#else
	pFunct = (CREATEPLUGIN)dlsym(dll.mod,"CreatePlugin");
#endif

	if(pFunct == nullptr)
	{
#ifdef _WIN32
		// error, corrupted dll
		FreeLibrary((HMODULE)dll.mod);
#else
		dlclose(dll.mod);
#endif

		sprintf_s(buffer,"CreatePlugin() function not found in: %s",pDLL);
		fm.WriteToLog(buffer);

		MessageBox(0,buffer,0,0);

		return nullptr;
	}

		
	// Create the plugin
	dll.pPlugin = pFunct(GetAngelScript().GetScriptEngine());
	assert(dll.pPlugin != nullptr);

	// Get the type of the plugin
	DLLType type = dll.pPlugin->GetPluginType();
	auto iter = m_plugins.find(type); // see if the plugin is already loaded

	// If it is already loaded
	if(iter != m_plugins.end())
	{
		// Free the old plugin
		FreePlugin(iter->second);

		// replace with new
		iter->second = dll;
	}
	else
	{

		// insert plugin into hash table
		m_plugins.insert(make_pair(type,dll));

		m_Keys.push_back(type);
	}

	// update log file
	sprintf_s(buffer,"(%s) is loaded",pDLL);
	fm.WriteToLog(buffer);

	// return the plugin interface
	return dll.pPlugin;
}

bool PluginManager::LoadAllPlugins(const std::string& path, const std::string& ext)
{
	vector<string> files;

	FileManager& fm = FileManager::Instance();
	fm.LoadAllFilesFromDirectory(files,path,ext);

	for_each(files.begin(),files.end(),[&](const string& str)
	{
		LoadDLL(str.c_str());
	});
		
	return !m_plugins.empty();
}

void PluginManager::FreePlugin(const PluginInfo& plugin)
{
	delete plugin.pPlugin;

#ifdef _WIN32
	FreeLibrary((HMODULE)plugin.mod);
#else
	dlclose(plugin.mod);
#endif
}

void PluginManager::FreePlugin(DLLType type)
{
	auto iter = m_plugins.find(type);
	if(iter != m_plugins.end())
	{
		FreePlugin(iter->second);

		m_plugins.erase(iter);

		auto posIter = std::find(m_Keys.begin(),m_Keys.end(),type);
		m_Keys.erase(posIter);
	}
}

/*void PluginManager::RegisterScript()
{
	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();

	DBAS(pEngine->RegisterObjectType("PluginManager",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterGlobalProperty("PluginManager pm",this));
}*/