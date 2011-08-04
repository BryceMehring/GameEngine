
// Programmed by Bryce Mehring

#ifndef _ASCONSOLE_
#define _ASCONSOLE_
#pragma once

#include "Interfaces.h"
#include "UI.h"

//class BEngine;

class asConsole : public TextBox, public IScripted
{
public:

	asConsole(UIData* pData, UI* pUI);
	~asConsole();

	// sends message to console window
	static void MessageCallback(const asSMessageInfo *msg, void *param);

	virtual void Enter();

	void Build();

	void Open();
	void Close();

private:

	// ===== data members =====	
	class BEngine* m_pEngine;

	static asConsole* s_pThis;

	// ===== helper functions =====

	// IScripted
	virtual void RegisterScript();

	void Run();

	// function declarations
	void PrintHelp() const;	
};

#endif