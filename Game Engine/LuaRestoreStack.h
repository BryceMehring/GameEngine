// LIBRARY INCLUDE FILES
#ifndef _LUA_RESTORE_STACK_H_
#define _LUA_RESTORE_STACK_H_
#pragma once
#include "LuaMachine.h"

class LuaRestoreStack
{
public:
	LuaRestoreStack (LuaMachine& vm) : m_pState (NULL)
	{
		m_pState = (lua_State *) vm;
		m_iTop = lua_gettop (m_pState);
	}
	~LuaRestoreStack ()
	{
		lua_settop (m_pState, m_iTop);
	}
protected:
	lua_State *m_pState;
	int m_iTop;
};

#endif // _LUA_RESTORE_STACK_H_