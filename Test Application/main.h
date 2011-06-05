// programmed by Bryce Mehring

#pragma once
#include "BEngine.h"
#include "PluginManager.h"
#include "UI.h"
#include <iostream>

#pragma comment(lib,"Game Engine.lib")

class InputTestApp : public IBaseEngine
{
public:

	// constructor/destructor
	InputTestApp(HINSTANCE hInstance,const std::string& winCaption);
	virtual ~InputTestApp();

	// from IBaseEngine
	
	// todo: need to implement these functions
	virtual void OnLostDevice() {}
	virtual void OnResetDevice() {}

	virtual int Run();

protected:

	// ===== data members =====

	// ui
	UI* m_pUI;

	char buffer[64];

	// ===== helper functions =====

	

};