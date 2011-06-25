// Programmed by Bryce Mehring

/*
 This class will manage the Execution of all scripts
 There will only be a single instance of the asVM class. And it
 will be publicly accessible.

 Documentation:
 file:///C:/Users/Bryce/Documents/C++/angelscript%20sdk/docs/manual/index.html

*/
#ifndef _ASMANAGER_
#define _ASMANAGER_
#pragma once

#include "Singleton.h"
#include "Interfaces.h"

// debug AngelScript
#ifdef _DEBUG
#ifndef DBAS 
#define DBAS(f) \
{ \
	int r = f; \
	assert( r >= 0 ); \
}
#endif // DBAS

#else
#ifndef DBAS 
#define DBAS(f) ((void)0)
#endif
#endif

// todo: Need to rename the namespace

namespace AngelScript
{


struct ScriptFunction
{
	int ifuncId;
	unsigned int iScriptIndex; // should this be registered with as?
};

// Internal structure in asVM for holding contents
struct Script;

class asVM : public IScripted
{
public:

	// in the constructor, the engine is created and is registered with the std::string type
	// the message callback function is also registered along with a global print function
	// for strings
	asVM();

	// the destructor releases all script contexts and then releases the engine
	virtual ~asVM();

	// Returned int is the id to the script
	// Build script and then add to vector
	unsigned int BuildScriptFromFile(const std::string& file);

	/* m_iExeScript is assigned the value of scriptId. Then the script is:
		1. Prepared ( push data on stack)
		2. Executed
		3. Unprepared (destroy stack)
	*/
	void ExecuteScript(unsigned int scriptId);
	void ExecuteScript(const char* script); // the script code is inserted within a function

	void ExecuteScriptFunction(unsigned int scriptId, int funcId);
	void ExecuteScriptFunction(unsigned int scriptId, int funcId, char param);

	void RemoveScript(unsigned int id);

	// returns a token from the input string from the script engine
	asETokenClass GetToken(std::string& token, const std::string& text, unsigned int& pos);
	
	// access to the asIScriptEngine
	asIScriptEngine& GetScriptEngine() const;

	virtual void RegisterScript();

private:

	// interfaces 
	asIScriptEngine* m_pEngine;

	// m_iExeScript is the script that is currently being executed. This variable is
	// being shared with AngelScript. But the script cannot modify this variable.
	unsigned int m_iExeScript;

	typedef std::map<std::string,unsigned int> ScriptIndexType;
	typedef std::vector<Script> ScriptElementType;

	// m_scriptIndex maps the filename of the script to the m_scripts index.
	ScriptIndexType m_scriptIndex;

	// each element in the vector is the main function in script
	ScriptElementType m_scripts;

	CScriptBuilder m_builder;

	// ===== helper functions =====
	void ListVariables();
	void ListObjects();
	void ListFunctions();
	bool GoodScriptId(unsigned int id) const; // returns true if the id is valid
	ScriptFunction GetFunc(asIScriptFunction* func) const;
	
};

}; // AngelScript namespace

#endif // _ASMANAGER_