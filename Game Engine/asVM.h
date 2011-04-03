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
#include <map>
#include <vector>
#include "scriptbuilder.h"
#include "singleton.h"

// Internal structure for holding contents
struct Script;

// There is only one instance of this class because there needs to be only
// one asIScriptEngine* instance.
class asVM : public Singleton<asVM>
{
public:

	//
	void ExecuteScript(unsigned int id);

	void RegisterScript(const char* file);

	unsigned int BuildScriptFromFile(const char*);
	unsigned int BuildScriptFromMemory(const char*);

	//unsigned int 

	// access to the asIScriptEngine
	asIScriptEngine* GetScriptEngine() const;

private:

	asIScriptEngine* m_pEngine;
	std::vector<Script> m_scripts;
	CScriptBuilder m_builder;

	asVM();
	~asVM();

	// helper functions
	unsigned int AddScript();
	
	friend class Singleton<asVM>;

};

#endif // _ASMANAGER_