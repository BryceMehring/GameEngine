#ifndef __LUA_SCRIPT_BASE_H__
#define __LUA_SCRIPT_BASE_H__
#pragma once
 
#include "LuaMachine.h"
 
class LuaScript
{
public:
	LuaScript (LuaMachine& vm);
	virtual ~LuaScript ();
   
	// Compile script into Virtual Machine
	bool CompileFile (const char *strFilename);
	bool CompileBuffer (unsigned char *pbBuffer, size_t szLen);
   
	// Register function with Lua
	int RegisterFunction (const char *strFuncName);
  
	// Selects a Lua Script function to call
	bool SelectScriptFunction (const char *strFuncName);
	void AddParam (int iInt);
	void AddParam (float fFloat);
	void AddParam (char *string);
   
	// Runs the loaded script
	bool Go (int nReturns = 0);
   
	// Checks on Virtual Machine script
	bool ScriptHasFunction (const char *strScriptName);
   
	// Method indexing check
	int methods () { return m_nMethods; }
      
	// When the script calls a class method, this is called
	virtual int ScriptCalling (LuaMachine& vm, int iFunctionNumber) = 0;
   
	// When the script function has returns
	virtual void HandleReturns (LuaMachine& vm, const char *strFunc) = 0;
	
	LuaMachine& vm () { return m_vm; }
   
protected:
     
	int m_nMethods;
	LuaMachine& m_vm;
	int m_iThisRef;
	int m_nArgs;
	const char *m_strFunctionName;
};

#endif // __LUA_SCRIPT_BASE_H__