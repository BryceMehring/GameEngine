// Programmed By Bryce Mehring
// 1/20/2011

// Read chapter 16, use the dynamic object mapper with the DLL files.
#include "WindowManager.h"
#include "PluginManager.h"
#include "FileManager.h"
//#include "EngineHelper.h"

using namespace std;
using namespace stdext;

PluginManager::PluginManager() : m_pEngine(nullptr)
{
}

// ===== Destructor =====
PluginManager::~PluginManager()
{
	FreeAllPlugins();
	m_pEngine = nullptr;
}

void PluginManager::FreeAllPlugins()
{
	for_each(m_plugins.begin(),m_plugins.end(),[&](const plugin_type::value_type& dll)
	{
		FreePlugin(dll.second);
	});

	m_plugins.clear();
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
/*asVM& PluginManager::GetScriptVM() const
{
	//return m_pEngine->GetScriptVM();
}*/

IPlugin* PluginManager::GetPlugin(DLLType type) const
{
	plugin_type::const_iterator iter = m_plugins.find(type);

	if(iter != m_plugins.end())
	{
		return iter->second.pPlugin;
	}

	return nullptr;
}

IPlugin* PluginManager::LoadDLL(const char* pDLL)
{
	FileManager& fm = FileManager::Instance();
	char buffer[128];
	PluginInfo dll = {0,0};

	dll.mod = LoadLibrary(pDLL);

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

	CREATEPLUGIN pFunct = (CREATEPLUGIN)GetProcAddress(dll.mod,"CreatePlugin");

	if(pFunct == nullptr)
	{
		// error, corrupted dll
		FreeLibrary(dll.mod);

		sprintf_s(buffer,"CreatePlugin() function not found in: %s",pDLL);
		fm.WriteToLog(buffer);

		MessageBox(0,buffer,0,0);

		return nullptr;
	}

		
	// Create the plugin
	dll.pPlugin = pFunct(*this);

	// todo: look into this
	//asIScriptEngine* pScriptEngine = m_pEngine->GetScriptVM()->GetScriptEngine();
	//pScriptEngine->RegisterGlobalProperty("

	// Get the type of the plugin
	DLLType type = dll.pPlugin->GetType();
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
	fm.LoadAllFilesFromDictionary(files,path,ext);

	for_each(files.begin(),files.end(),[&](const string& str)
	{
		LoadDLL(str.c_str());
	});
		
	return !m_plugins.empty();
}

void PluginManager::FreePlugin(const PluginInfo& plugin)
{
	delete plugin.pPlugin;
	FreeLibrary(plugin.mod);
}

void PluginManager::FreePlugin(DLLType type)
{
	plugin_type::iterator iter = m_plugins.find(type);
	if(iter != m_plugins.end())
	{
		FreePlugin(iter->second);
	}
}

/*void PluginManager::RegisterScript()
{
	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();

	DBAS(pEngine->RegisterObjectType("PluginManager",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterGlobalProperty("PluginManager pm",this));
}*/