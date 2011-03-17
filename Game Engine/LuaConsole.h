// Programmed By Bryce Mehring

#pragma once
#include <Windows.h>
#include "LuaMachine.h"
#include "LuaScript.h"


class LuaConsole : public LuaScript
{
public:

	LuaConsole(LuaMachine& vm) : LuaScript(vm) {}
	~LuaConsole() {}

	void OpenConsole(bool);

	void TestRun()
	{
		
	}

	int Clear(LuaMachine& vm)
	{
		system("cls");
		return 0;
	}

private:

	// console window
	HANDLE m_luaConsole; // console thread
	HWND m_hConsole; // console window

};