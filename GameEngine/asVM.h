// Programmed by Bryce Mehring

/*
 This class will manage the Execution of all scripts

 Documentation:
 file:///C:/Users/Bryce/Documents/C++/angelscript%20sdk/docs/manual/index.html

*/
#ifndef _ASMANAGER_
#define _ASMANAGER_

#include "../angelscript/scriptstdstring/scriptstdstring.h"
#include "../angelscript/scriptbuilder/scriptbuilder.h"
#include <cassert>

// debug AngelScript macro
/*#ifdef _DEBUG
#ifndef DBAS 
#define DBAS(f) \
{ \
	int r = f; \
	assert( r >= 0); \
}
#endif // DBAS

#else*/
#ifndef DBAS 
#define DBAS(f) (f)
#endif

struct ScriptFunctionStruct
{
	int ifuncId;
	unsigned int iScriptIndex; // should this be registered with as?
};

// ===== forward class declarations =====

// Internal structure in asVM for holding contents
struct Script;

template< class R, class A >
class Delegate;

//template< class T >
//class IScripted;

// ===== forward class declarations =====

class asVM
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
	void ExecuteScript(const std::string& script, asDWORD mask); // the script code is inserted within a function

	void ExecuteScriptFunction(const ScriptFunctionStruct& func);
	void ExecuteScriptFunction(unsigned int scriptId, int funcId, char param);

	void RemoveScript(unsigned int id);

	// returns a token from the input string from the script engine
	//asETokenClass GetToken(std::string& token, const std::string& text, unsigned int& pos);

	void RegisterScript(class ScriptingConsole* pTextBox);

	// access to the asIScriptEngine
	asIScriptEngine* GetScriptEngine() const;

	void SendMessage(const std::string& msg) const;

	void Save() const;

private:

	typedef Delegate<void,void> asDelegate;

	// interfaces 
	asIScriptEngine* m_pEngine;

	// m_iExeScript is the script that is currently being executed. This variable is
	// being shared with AngelScript. But the script cannot modify this variable.
	unsigned int m_iExeScript;

	// each element in the vector is the main function in script
	typedef std::vector<Script> ScriptElementType;
	ScriptElementType m_scripts;

	CScriptBuilder m_builder;

	// ===== helper functions =====
	void ListVariables();
	void ListObjects();
	void ListFunctions();
	bool GoodScriptId(unsigned int id) const; // returns true if the id is valid

	// called from the script, 
	//asDelegate GetFunc(asIScriptFunction* func);

	//class VoidDelegate CreateDelegate(ScriptFunction func) const;
	
};

#endif // _ASMANAGER_
