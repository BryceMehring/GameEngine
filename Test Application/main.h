// programmed by Bryce Mehring

#pragma once
#include "BEngine.h"
#include "PluginManager.h"


#pragma comment(lib,"Game Engine.lib")

class InputTestApp : public IBaseEngine
{
public:

	// constructor/destructor
	InputTestApp(HINSTANCE hInstance,const std::string& winCaption);
	virtual ~InputTestApp();

	//virtual void MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam);

	virtual int Run();

protected:

	// ===== data members =====

	std::string m_str;
	char buffer[64];

	// ===== helper functions =====
	void LoadAndExecScripts();	

};