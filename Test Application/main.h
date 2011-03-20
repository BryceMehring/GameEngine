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

	virtual int Run();

protected:

	IKMInput* m_pInput;
	IRenderingPlugin* m_pRendering;

	char buffer[64];

	void LoadDLLS();

	// constructor/destructor
	InputTestApp(HINSTANCE hInstance,const string& winCaption);
	virtual ~InputTestApp();

	friend class IBaseEngine;

};