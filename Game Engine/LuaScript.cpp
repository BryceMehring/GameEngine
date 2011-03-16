// ---------------------------------------------------------------------------
// FILE NAME            : LuaScript.cpp
// ---------------------------------------------------------------------------
// DESCRIPTION :
//
// 
// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include "LuaScript.h"
#include "LuaThis.h"
#include "LuaRestoreStack.h"
#include <assert.h>
// ---------------------------------------------------------------------------

//============================================================================
// int LuaCallback
//---------------------------------------------------------------------------
// Lua C-API calling that figures out which object to hand over to
//
// Parameter   Dir      Description
// ---------   ---      -----------
// lua         IN       State variable
//
// Return
// ------
// Number of return variables on the stack
//
// Comments
// --------
// This is the function lua calls for each C-Function that is
// registered with lua. At the time of registering the function
// with lua, we make lua record the method number so we can
// know what method was actually called. The lua stack is the
// following structure:
// 0: 'this' (table)
 // 1 - ...: parameters passed in
//============================================================================
static int LuaCallback (lua_State *lua)
{
   // Locate the pseudo-index for the function number
   int iNumberIdx = lua_upvalueindex (1);
   int nRetsOnStack = 0;

   bool fSuccess = false;

   // Check for the this table
   if (lua_istable (lua, 1))
   {
      // Found the this table. The object pointer is at the index 0
      lua_rawgeti (lua, 1, 0);
      
      if (lua_islightuserdata (lua, -1))
      {
         // Found the pointer, need to cast it
         LuaScript *pThis = (LuaScript *) lua_touserdata (lua, -1);

         // Get the method index
         int iMethodIdx = (int) lua_tonumber (lua, iNumberIdx);

         // Check that the method is correct index
        assert (!(iMethodIdx > pThis->methods ()));

         // Reformat the stack so our parameters are correct
         // Clean up the this table
         lua_remove (lua, 1);
         // Clean up the pThis pointer
         lua_remove (lua, -1);

         // Call the class
        nRetsOnStack = pThis->ScriptCalling (pThis->vm (), iMethodIdx);

         fSuccess = true;
      }
   }

   if (fSuccess == false)
   {
		lua_pushstring (lua, "LuaCallback -> Failed to call the class function");
		lua_error (lua);
   }

   // Number of return variables
   return nRetsOnStack;
}


//============================================================================
// LuaScript::LuaScript
//---------------------------------------------------------------------------
// Constructor. Sets up the lua stack and the &quot;this&quot; table
//
// Parameter            Dir      Description
// ---------            ---      -----------
// CLuaVirtualMachine   IN       VM to run on
//
// Return
// ------
// None.
//
//============================================================================
LuaScript::LuaScript (LuaMachine& vm)
 : m_vm (vm), m_nMethods (0), m_iThisRef (0), m_nArgs (0)
{
	lua_State *state = (lua_State *) m_vm;
	
	// Create a reference to the this table. Each reference is unique
	lua_newtable (state);
	m_iThisRef = luaL_ref (state, LUA_REGISTRYINDEX);

	// Save the this table to index 0 of the this table
	//CLuaRestoreStack rs (vm);
	lua_rawgeti (state, LUA_REGISTRYINDEX, m_iThisRef);
	lua_pushlightuserdata (state, (void *) this);
	lua_rawseti (state, -2, 0);

}

//============================================================================
// LuaScript::~LuaScript
//---------------------------------------------------------------------------
// Destructor
//
// Parameter   Dir      Description
// ---------   ---      -----------
// None.
//
// Return
// ------
// None.
//
//============================================================================
LuaScript::~LuaScript (void)
{
	//CLuaRestoreStack rs (m_vm);

	lua_State *state = (lua_State *) m_vm;

	// Get the reference this table
	lua_rawgeti (state, LUA_REGISTRYINDEX, m_iThisRef);

	// Clear index 0
	lua_pushnil (state);
	lua_rawseti (state, -2, 0);
}

//============================================================================
// bool LuaScript::CompileBuffer
//---------------------------------------------------------------------------
// Compiles a given buffer
//
// Parameter   Dir      Description
// ---------   ---      -----------
// pbBuffer    IN       Data buffer
// szLen       IN       Length of buffer
//
// Return
// ------
// Success
//
//============================================================================
bool LuaScript::CompileBuffer (unsigned char *pbBuffer, size_t szLen)
{
	// Make sure we have the correct this table
	LuaThis luaThis (m_vm, m_iThisRef);

	return m_vm.RunBuffer (pbBuffer, szLen);
}

//============================================================================
// bool LuaScript::CompileBuffer
//---------------------------------------------------------------------------
// Compiles a given file
//
// Parameter   Dir      Description
// ---------   ---      -----------
// strFilename IN       File name to compile
//
// Return
// ------
// Success
//
//============================================================================
bool LuaScript::CompileFile (const char *strFilename)
{
   // Make sure we have the correct this table
   LuaThis luaThis (m_vm, m_iThisRef);

   return m_vm.RunFile (strFilename);
}

//============================================================================
// int LuaScript::RegisterFunction
//---------------------------------------------------------------------------
// Registers a function with Lua
//
// Parameter   Dir      Description
// ---------   ---      -----------
// strFuncName IN       Function name
//
// Return
// ------
// Success
//
//============================================================================
int LuaScript::RegisterFunction (const char *strFuncName)
{
	LuaRestoreStack rs (m_vm);

	lua_State *state = (lua_State *) m_vm;
	++m_nMethods;

	// Register a function with the lua script. Added it to the this table
	lua_rawgeti (state, LUA_REGISTRYINDEX, m_iThisRef);

	// Push the function and parameters
	lua_pushstring (state, strFuncName);
	lua_pushnumber (state, (lua_Number) m_nMethods);
	lua_pushcclosure (state, LuaCallback, 1);
	lua_settable (state, -3);

	return m_nMethods;
}

//============================================================================
// bool LuaScript::SelectScriptFunction
//---------------------------------------------------------------------------
// Selects a script function to run
//
// Parameter   Dir      Description
// ---------   ---      -----------
// strFuncName IN       Function name
//
// Return
// ------
// Success
//
//============================================================================
bool LuaScript::SelectScriptFunction (const char *strFuncName)
{
	bool fSuccess = true;

	lua_State *state = (lua_State *) m_vm;
	// Look up function name
	lua_rawgeti (state, LUA_REGISTRYINDEX, m_iThisRef);
	lua_pushstring (state, strFuncName);
	lua_rawget (state, -2);
	lua_remove (state, -2);

	// Put the this table back
	lua_rawgeti (state, LUA_REGISTRYINDEX, m_iThisRef);

	// Check that we have a valid function
	if (!lua_isfunction (state, -2))
	{
		fSuccess = false;
		lua_pop (state, 2);
	}
	else
	{
		m_nArgs = 0;
		m_strFunctionName = strFuncName;
	}
   
	return fSuccess;
}

//============================================================================
// bool LuaScript::ScriptHasFunction
//---------------------------------------------------------------------------
// Checks to see if a function exists
//
// Parameter      Dir      Description
// ---------      ---      -----------
// strScriptName  IN       Function name
//
// Return
// ------
// Success
//
//============================================================================
bool LuaScript::ScriptHasFunction (const char *strScriptName)
{
	LuaRestoreStack rs (m_vm);

	bool fFoundFunc = false;

	lua_State *state = (lua_State *) m_vm;
	lua_rawgeti (state, LUA_REGISTRYINDEX, m_iThisRef);
	lua_pushstring (state, strScriptName);
	lua_rawget (state, -2);
	lua_remove (state, -2);

	if (lua_isfunction (state, -1))
	{
		fFoundFunc = true;
	}

	return fFoundFunc;
}

//============================================================================
// void LuaScript::AddParam
//---------------------------------------------------------------------------
// Adds a parameter to the parameter list
//
// Parameter   Dir      Description
// ---------   ---      -----------
// string      IN       string param
//
// Return
// ------
// None.
//
//============================================================================
void LuaScript::AddParam (char *string)
{
	lua_State *state = (lua_State *) m_vm;
	lua_pushstring (state, string);
	++m_nArgs;
}

//============================================================================
// void LuaScript::AddParam
//---------------------------------------------------------------------------
// Adds a parameter to the parameter list
//
// Parameter   Dir      Description
// ---------   ---      -----------
// iInt        IN       int param
//
// Return
// ------
// None.
//
//============================================================================
void LuaScript::AddParam (int iInt)
{
 
 lua_State *state = (lua_State *) m_vm;
      lua_pushnumber (state, (lua_Number) iInt);
      ++m_nArgs;
  
}

//============================================================================
// void LuaScript::AddParam
//---------------------------------------------------------------------------
// Adds a parameter to the parameter list
//
// Parameter   Dir      Description
// ---------   ---      -----------
// fFloat      IN       float param
//
// Return
// ------
// None.
//
//============================================================================
void LuaScript::AddParam (float fFloat)
{
 
 lua_State *state = (lua_State *) m_vm;
      lua_pushnumber (state, (lua_Number) fFloat);
      ++m_nArgs;
 
}

//============================================================================
// bool LuaScript::Go
//---------------------------------------------------------------------------
// Runs the selected script function
//
// Parameter   Dir      Description
// ---------   ---      -----------
// nReturns    IN       Number of expected returns
//
// Return
// ------
// None.
//
//============================================================================
bool LuaScript::Go (int nReturns )
{
  
   // At this point there should be a parameters and a function on the
   // Lua stack. Each function get a this parameter as default and is
   // pushed onto the stack when the method is selected

   bool fSuccess = m_vm.CallFunction (m_nArgs + 1, nReturns);

   if (fSuccess == true && nReturns > 0)
   {
      // Check for returns
      HandleReturns (m_vm, m_strFunctionName);
      lua_pop ((lua_State *) m_vm, nReturns);
   }

   return fSuccess;
}