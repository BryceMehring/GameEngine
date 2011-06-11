// programmed by Bryce Mehring

#pragma once
#include "BEngine.h"
#include "PluginManager.h"
#include "UI.h"

#pragma comment(lib,"Game Engine.lib")

class InputTestApp : public IBaseEngine
{
public:

	// constructor/destructor
	InputTestApp(HINSTANCE hInstance,const std::string& winCaption);
	virtual ~InputTestApp();

	virtual int Run();

protected:

	// ===== data members =====

	// ui
	boost::scoped_ptr<UI> m_pUI;

	char buffer[64];

	// ===== helper functions =====

	

};