
// Programmed by Bryce Mehring

#ifndef _ASCONSOLE_
#define _ASCONSOLE_
#pragma once

/*#include <Windows.h>
#include <string>
#include "Singleton.h"

DWORD WINAPI ConsoleThread(void*);

class asConsole : public Singleton<asConsole>
{
public:

	void Open();
	void Close();

private:

	asConsole();
	~asConsole();

	// ===== data members =====

	bool m_bOpen;
	unsigned int m_iScript;

	// Scripting Console
	HWND m_hConsole;
	HANDLE m_ConsoleThread;

	// ===== helper functions =====

	DWORD Run();

	// function declarations
	void ListVariables() const;
	void ListFunctions() const;
	void PrintHelp() const;
	void ConsoleInfo() const;
	void CLS() const;
	void InterpretLine(std::string& str);
	void AddVariable(std::string &arg);
	void AddFunction(std::string &arg);
	void ExecuteScript();

	friend class Singleton<asConsole>;
	friend DWORD WINAPI ConsoleThread(void*);
};

#endif*/