

#include "FileManager.h"
#include "asVM.h"
#include "GameConstants.h"

using namespace std;

FileManager::~FileManager()
{
	::fstream out(Constants::LOGFILE,ios::out);

	if(out.is_open())
	{
		out << m_buffer.str() << endl;
		out.close();
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
