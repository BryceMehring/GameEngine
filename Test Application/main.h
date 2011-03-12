// programmed by Bryce Mehring

#pragma once
#include "BEngine.h"
#include "PluginManager.h"

class InputTestApp : public IBaseEngine
{
public:

	InputTestApp(HINSTANCE hInstance,const string& winCaption) : IBaseEngine(hInstance,winCaption)
	{
		PluginManager& pluginManager = PluginManager::Instance();
		m_pInput  = static_cast<IKMInput*>(pluginManager.LoadDLL("DirectX Input DLL.dll"));
		ZeroMemory(buffer,32);
	}

	virtual int Run()
	{
		while(Update())
		{
			m_pInput->Poll();
			mouseXCoord += m_pInput->MouseX();
			mouseYCoord += m_pInput->MouseY();

			if(m_pInput->MouseClick(0))
			{
				wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",mouseXCoord,mouseYCoord);
			}

			//m_pRendering->DrawText(buffer);
			//m_pRendering->Present();
		}

		return 0;
	}

protected:

	IKMInput* m_pInput;
	IRenderingPlugin* m_pRendering;

	int mouseYCoord;
	int mouseXCoord;
	char buffer[64];

	~InputTestApp() {}

};