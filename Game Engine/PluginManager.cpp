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
	for(plugin_type::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		delete iter->second.pPlugin;
		FreeLibrary(iter->second.mod);
	}
	m_pEngine = nullptr;
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
	IPlugin* pPlugin = nullptr;
	plugin_type::const_iterator iter = m_plugins.find(type);

	if(iter != m_plugins.end())
	{
		pPlugin = iter->second.pPlugin;
	}

	return pPlugin;
}

IPlugin* PluginManager::LoadDLL(const char* pDLL)
{
	PluginInfo dll = {0,0};
	dll.mod = LoadLibrary(pDLL);

	if(dll.mod)
	{
		CREATEPLUGIN pFunct = (CREATEPLUGIN)GetProcAddress(dll.mod,"CreatePlugin");
		dll.pPlugin = pFunct(*this);

		// todo: look into this
		//asIScriptEngine* pScriptEngine = m_pEngine->GetScriptVM()->GetScriptEngine();
		//pScriptEngine->RegisterGlobalProperty("

		DLLType type = dll.pPlugin->GetType();

		m_plugins.insert(make_pair(type,dll));
	}
	else
	{
		// need to output message to log
		// todo: should create a logging singleton that should interact with the console.
		cout<< endl << "Could not load: " << pDLL << endl;
	}

	return dll.pPlugin;
}

bool PluginManager::LoadAllPlugins(const std::string& path, const std::string& ext)
{
	vector<string> files;

	FileManager& fm = FileManager::Instance();
	fm.LoadAllFilesFromDictionary(files,path,ext);

	for(unsigned int i = 0; i < files.size(); ++i)
	{
		LoadDLL(files[i].c_str());
	}

	return (m_plugins.size() > 0);
}

void PluginManager::FreePlugin(DLLType type)
{
	plugin_type::iterator iter = m_plugins.find(type);
	if(iter != m_plugins.end())
	{
		PluginInfo& dll = iter->second;

		delete dll.pPlugin;
		FreeLibrary(dll.mod);
	}
}

/*void PluginManager::RegisterScript()
{
	asVM* pVM = asVM::Instance();
	asIScriptEngine* pEngine = pVM->GetScriptEngine();

	DBAS(pEngine->RegisterObjectType("PluginManager",0,asOBJ_REF | asOBJ_NOHANDLE));
	DBAS(pEngine->RegisterGlobalProperty("PluginManager pm",this));
}*/