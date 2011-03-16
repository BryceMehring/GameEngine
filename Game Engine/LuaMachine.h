// Programmed by Bryce Mehring

#ifndef _LUA_MACHINE_
#define _LUA_MACHINE_
#pragma once

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

// This class manages the (lua_State*)
class LuaMachine
{
public:

	LuaMachine();
	~LuaMachine();

	// Load and run script elements
	bool RunFile (const char *strFilename);
	bool RunBuffer (const unsigned char *pbBuffer, size_t szLen, const char *strName = NULL);

	// C-Api into script
	bool CallFunction (int nArgs, int nReturns = 0);

	static void Panic (lua_State *lua);

	operator lua_State*();

private:

	lua_State* m_state;

};

#endif