
#ifndef _LUA_THIS_
#define _LUA_THIS_
#pragma once
   
#include "LuaMachine.h"

class LuaThis
{
public:

	LuaThis (LuaMachine& vm, int iRef) : m_iOldRef (0), m_vm (vm)
	{
		lua_State *state = (lua_State *) vm;
		
		// Save the old "this" table
		lua_getglobal (state, "this");
		m_iOldRef = luaL_ref (state, LUA_REGISTRYINDEX);
   
		// replace it with our new one
		lua_rawgeti(state, LUA_REGISTRYINDEX, iRef);
		lua_setglobal (state, "this");
	}
	~LuaThis ()
	{
		lua_State *state = (lua_State *) m_vm;
		if (m_iOldRef > 0)
		{
			// Replace the old "this" table
			lua_rawgeti(state, LUA_REGISTRYINDEX, m_iOldRef);
			lua_setglobal (state, "this");
			luaL_unref (state, LUA_REGISTRYINDEX, m_iOldRef);
		
		}
		
	}

private:
	int m_iOldRef;
	LuaMachine& m_vm;
};

#endif _LUA_THIS_