// Programmed by Bryce Mehring

/*
 This class will manage the Execution of all scripts
 There will only be a single instance of the asVM class. And it
 will be publicly accessible.

*/
#ifndef _ASMANAGER_
#define _ASMANAGER_
#pragma once

#include <angelscript.h>
#include "Singleton.h"
#include "ScriptBuilder.h"
#include "scriptstdstring.h"
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <Windows.h>

// todo: need to add everything into its own namespace.

namespace AngelScript
{

#ifndef DBAS // debug AngelScript
#define DBAS(f) \
{ \
	int r = f; \
	assert( r >= 0 ); \
}
#endif // DBAS


// ===== registering helpers ======

// todo: should I combine all of these helpers into one class?

template< class A, class B >
B* refCast(A* a)
{
    // If the handle already is a null handle, then just return the null handle
    if( a == NULL ) return 0;

    // Now try to dynamically cast the pointer to the wanted type
    B* b = dynamic_cast<B*>(a);
    if( b != NULL )
    {
        // Since the cast was made, we need to increase the ref counter for the returned handle
        b->AddRef();
    }
    return b;
}

template< class T >
void Construct(void* pMem)
{
	new(pMem) T();
}

template< class T >
void CopyConstruct(const T& other, T* pThis)
{
	new(pThis) T(other);
}

template< class T >
void Destroy(void* pMem)
{
	((T*)pMem)->~T();
}

// ================



// ======================


// Todo: answer these questions:
// Why do we need this class? What does it mean when singletons inherit from the class?
class IScripted
{
public:

	virtual ~IScripted() {}
	virtual void RegisterScript() = 0;
	// todo: maybe add a deregister method?

};

// Internal structure in asVM for holding contents
struct Script;

// There is only one instance of this class because there needs to be only
// one asIScriptEngine* instance.
class asVM : public Singleton<asVM>, public IScripted
{
public:

	// Returned int is the id to the script
	// Build script and then add to vector
	unsigned int BuildScriptFromFile(const std::string& str);

	/* m_iExeScript is assigned the value of scriptId. Then the script is:
		1. Prepared ( push data on stack)
		2. Executed
		3. Unprepared (destroy stack)
	*/
	void ExecuteScript(unsigned int scriptId);

	void ExecuteScriptFunction(unsigned int scriptId, int funcId);
	void ExecuteScriptFunction(unsigned int scriptId, int funcId, char param);

	// Release script, then remove from vector
	void RemoveScript(unsigned int id);

	// returns a token from the input string from the script engine
	asETokenClass GetToken(std::string& token, const std::string& text, unsigned int& pos);
	
	// access to the asIScriptEngine
	asIScriptEngine* GetScriptEngine() const;

	virtual void RegisterScript();

private:

	asIScriptEngine* m_pEngine;

	typedef std::map<std::string,unsigned int> ScriptIndexType;

	// m_iExeScript is the script that is currently being executed. This variable is
	// being shared with AngelScript. But the script cannot modify this variable.
	unsigned int m_iExeScript;

	// m_scriptIndex maps the filename of the script to the m_scripts index.
	std::map<std::string,unsigned int> m_scriptIndex;

	std::vector<Script> m_scripts;

	CScriptBuilder m_builder;

	// constructor/destructor

	// in the constructor, the engine is created and is registered with the std::string type
	// the message callback function is also registered along with a global print function
	// for strings
	asVM();

	// the destructor releases all script contexts and then releases the engine
	virtual ~asVM();

	// ===== helper functions =====
	
	///...

	// friends
	friend class Singleton<asVM>;

};

}; // AngelScript namespace

#endif // _ASMANAGER_