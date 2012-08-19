#include "SerializedState.h"
#include "asVM.h"
#include "FileManager.h"
#include "StringAlgorithms.h"

#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

SerializedState::SerializedState(const char* pFile) : m_pFile(pFile) {}

void SerializedState::Load(asVM& vm)
{
	vm.SendMessage((string(m_pFile) + " Config loaded").c_str());

	// open the file
	FileManager::Instance().ProccessFileByLine(m_pFile,[&](const string& line)
	{
		if(line.empty() == false)
		{
			// Add the code to the map, and exec the code. 
			// note: the script code gets executed even if the line did not get inserted into the map
			// note: this allows for the potential for script code to be executed during the loading of a state.
			BuildMap(line);

			vm.ExecuteScript(line,0);
			vm.SendMessage(line.c_str());
		}
	});
}

void SerializedState::SaveMap() const
{
	std::fstream out(m_pFile,std::fstream::out);
	
	for(auto iter = m_scriptFile.begin(); iter != m_scriptFile.end(); ++iter)
	{
		out << iter->first << "=" << iter->second << endl;
	}

	out.close();
}

void SerializedState::BuildMap(const string& line)
{
	unsigned int uiCounter = 0;
	string variables[2];
	stringstream stream(line); 

	while(stream && uiCounter < 2)
	{
		if(!getline(stream,variables[uiCounter],'=')) { break; }

		string& ref = variables[uiCounter];
		ref.erase(remove_if(ref.begin(), ref.end(),isspace), ref.end());

		++uiCounter;
	}

	m_scriptFile.insert(make_pair(variables[0],variables[1]));
}
