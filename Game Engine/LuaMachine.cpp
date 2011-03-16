// Programmed by Bryce Mehring

#include "LuaMachine.h"

#pragma comment(lib,"lua51.lib")

LuaMachine::LuaMachine() : m_state(lua_open())
{
	luaL_openlibs(m_state);
}
LuaMachine::~LuaMachine()
{
	lua_close(m_state);
}

LuaMachine::operator lua_State *()
{
	return m_state;
}

bool LuaMachine::RunFile (const char *strFilename)
{
    bool fSuccess = false;
    int iErr = 0;
  
	if ((iErr = luaL_loadfile (m_state, strFilename)) == 0)
	{
		// Call main...
		if ((iErr = lua_pcall (m_state, 0, LUA_MULTRET, 0)) == 0)
		{
			fSuccess = true;
		}
	}

    return fSuccess;
}

bool LuaMachine::RunBuffer (const unsigned char *pbBuffer, size_t szLen, const char *strName)
{
	bool fSuccess = false;
  
	if (luaL_loadbuffer (m_state, (const char *) pbBuffer, szLen, strName) == 0)
	{
		// Call main...
		if (lua_pcall (m_state, 0, LUA_MULTRET, 0) == 0)
		{
			fSuccess = true;
		}
	}
  
	return fSuccess;   
  
}

bool LuaMachine::CallFunction (int nArgs, int nReturns /* = 0 */)
{
	bool fSuccess = false;
     
	if (lua_isfunction (m_state, -nArgs-1))
	{
		if(lua_pcall (m_state, nArgs, nReturns, 0) == 0)
		{
			fSuccess = true;
		}
	}
  
	return fSuccess;
}