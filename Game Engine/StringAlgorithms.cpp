#include "StringAlgorithms.h"
#include "FileManager.h"
#include "asVM.h"
#include <algorithm>
#include <ctype.h>
#include <set>

using namespace std;

void StringToLower(std::string& str)
{
	std::transform(str.begin(),str.end(),str.begin(),tolower);
}

void StringToUpper(std::string& str)
{
	std::transform(str.begin(),str.end(),str.begin(),toupper);
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

unsigned int myHash(const char *str)
{
    unsigned int hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
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

float CompareFiles(const std::string& file1, const std::string& file2)
{
	const std::string files[2] = {file1,file2};
	std::vector<std::vector<unsigned int>> hashCodes;
	hashCodes.resize(2);

	for(unsigned int i = 0; i < 2; ++i)
	{
		::FileManager::Instance().ProccessFileByLine(files[i].c_str(),[&](const string& line)
		{
			hashCodes[i].push_back(myHash(line.c_str()));
		});
	}

	unsigned int uiMax = min(hashCodes[0].size(),hashCodes[1].size());
	const float uiPLine = 1.0f / uiMax;
	float p = 1.0f;
	for(unsigned int i = 0; i < uiMax; ++i)
	{
		if(hashCodes[0][i] != hashCodes[1][i])
		{
			p -= uiPLine;
		}
	}

	return p*100.0f;
}

void RegisterStrAlgorScript(asVM& vm)
{
	auto pEngine = vm.GetScriptEngine();

	//DBAS(pEngine->RegisterGlobalFunction("void lower(const string&inout)",asFUNCTION(StringToLower),asCALL_CDECL));
	//DBAS(pEngine->RegisterGlobalFunction("void upper(string &out)",asFUNCTION(StringToUpper),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("uint StringHash(const string& in)",asFUNCTION(myHash),asCALL_CDECL));
	DBAS(pEngine->RegisterGlobalFunction("float CompareFiles(const string& in,const string& in)",asFUNCTION(CompareFiles),asCALL_CDECL));
	//DBAS(pEngine->RegisterObjectMethod("Game","const string& GetCurrentState() const",asMETHOD(Game,GetCurrentState),asCALL_THISCALL));
	//DBAS(pEngine->RegisterGlobalProperty("Game game",(void*)this));

	pEngine->Release();
}