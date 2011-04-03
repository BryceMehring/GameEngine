
// Programmed by Bryce Mehring

#ifndef _ASCONSOLE_
#define _ASCONSOLE_
#pragma once

#include <Windows.h>
#include "Singleton.h"

DWORD WINAPI ConsoleThread(void* parameter);

class asConsole : public Singleton<asConsole>
{
public:

	void Open();
	void Close();

private:

	asConsole();
	~asConsole();

	// data members

	bool m_bOpen;

	// Scripting Console
	HWND m_hConsole;
	HANDLE m_ConsoleThread;

	// functions

	DWORD Run();

	void ListVariables();
	void ListFunctions();
	void PrintHelp();
	void ConsoleInfo();


	friend class Singleton<asConsole>;
	friend DWORD WINAPI ConsoleThread(void* parameter);
};

#endif