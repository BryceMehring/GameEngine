#include "StringAlgorithms.h"
#include "FileManager.h"
#include "asVM.h"
#include <algorithm>
#include <ctype.h>
#include <set>

using namespace std;

void StringToLower(std::string& str)
{
	std::transform(str.begin(),str.end(),str.begin(),::tolower);
}

void StringToUpper(std::string& str)
{
	//std::transform(str.begin(),str.end(),str.begin(),toupper);
}

std::string GetFileNameFromPath(const std::string& file)
{
	// get the bounds of the actual filename
	size_t start = file.find_last_of("/\\") + 1;
	size_t n = file.find_last_of('.') - start;

	return file.substr(start,n);
}

std::string WrapWithQuote(const std::string& str)
{
	return ('"' + str + '"');
}



void GetDelimitedString(const std::string& str, std::vector<std::string>& out, char delim)
{
	std::stringstream stream(str);
	string temp;

	while(getline(stream,temp,delim))
	{
		out.push_back(temp);
	}
}

void RegisterStrAlgorScript(asVM& vm)
{
	auto pEngine = vm.GetScriptEngine();

	//DBAS(pEngine->RegisterGlobalFunction("void lower(const string&inout)",asFUNCTION(StringToLower),asCALL_CDECL));
	//DBAS(pEngine->RegisterGlobalFunction("void upper(string &out)",asFUNCTION(StringToUpper),asCALL_CDECL));
	//DBAS(pEngine->RegisterGlobalFunction("uint StringHash(const string& in)",asFUNCTION(myHash),asCALL_CDECL));
	//DBAS(pEngine->RegisterGlobalFunction("float CompareFiles(const string& in,const string& in)",asFUNCTION(CompareFiles),asCALL_CDECL));
	//DBAS(pEngine->RegisterObjectMethod("Game","const string& GetCurrentState() const",asMETHOD(Game,GetCurrentState),asCALL_THISCALL));
	//DBAS(pEngine->RegisterGlobalProperty("Game game",(void*)this));

	pEngine->Release();
}
