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
#include "scriptbuilder.h"
#include "singleton.h"

class asVM : public Singleton<asVM>
{
public:

	void ExecuteScript(const char* name);

	void BuildScriptFromFile(const char* file, const char* name);
	void BuildScriptFromMemory();

private:

	asIScriptEngine* m_pEngine;
	CScriptBuilder m_builder;

	asVM();
	~asVM();
	
	friend class Singleton<asVM>;

};

#endif