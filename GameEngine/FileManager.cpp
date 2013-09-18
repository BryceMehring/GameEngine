

#include "FileManager.h"
#include "asVM.h"

using namespace std;

FileManager::FileManager()
{
	m_timer.Start();

	m_file.open("GameLog.txt");
	unsigned int counter = 2;
	while(!m_file.is_open())
	{
		std::stringstream stream;
		stream << counter++;

		m_file.open("GameLog_" + stream.str() + ".txt");
	}
}

FileManager::~FileManager()
{
	if(m_file.is_open())
	{
		m_file.close();
	}
}

void FileManager::RegisterScript(asVM& vm)
{
	auto pEngine = vm.GetScriptEngine();

	const char* NAME = "FileManager";

	DBAS(pEngine->RegisterObjectType(NAME,0,asOBJ_REF | asOBJ_NOHANDLE));

	//DBAS(pEngine->RegisterObjectMethod(NAME,"void LoadAllFilesFromDictionary(array<string>& out, const string& in, const string& in) const",asMETHOD(FileManager,LoadAllFilesFromDictionary),asCALL_THISCALL));
	//DBAS(pEngine->RegisterObjectMethod(NAME,"uint CountNumberOfLines(const string& in, const string& in) const",asMETHOD(FileManager,CountNumberOfLines),asCALL_THISCALL));
	//DBAS(pEngine->RegisterObjectMethod(NAME,"string OpenFileName() const",asMETHOD(FileManager,OpenFileName),asCALL_THISCALL));
	//DBAS(pEngine->RegisterObjectMethod(NAME,"bool GetFolder(string& out) const",asMETHOD(FileManager,GetFolder),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("FileManager fm",this));

	pEngine->Release();
}
