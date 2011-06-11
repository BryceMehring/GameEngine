
// Programmed by Bryce Mehring

#ifndef _ASCONSOLE_
#define _ASCONSOLE_
#pragma once

#include "Thread.h"

class asConsole : public IScripted, Threaded
{
public:

	asConsole();
	~asConsole();

	void Open();
	void Close();

	void CLS() const;

private:

	// ===== data members =====

	//asVM* m_pVM;

	// Scripting Console
	HWND m_hConsole;

	// ===== helper functions =====

	// IScripted
	virtual void RegisterScript();

	// Threaded
	virtual void DoWork();

	void Run();

	// function declarations
	void PrintHelp() const;
	void ConsoleInfo() const;
	
};

#endif