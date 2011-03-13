// programmed by Bryce Mehring

#pragma once
#include "BEngine.h"
#include "PluginManager.h"
#include <iostream>

class InputTestApp : public IBaseEngine
{
public:

	InputTestApp(HINSTANCE hInstance,const string& winCaption) : IBaseEngine(hInstance,winCaption)
	{
		mouseYCoord = 0;
		mouseXCoord = 0;

		ZeroMemory(buffer,32);
	}

	virtual int Run()
	{
		Load();

		RedirectIOToConsole();

		char Buffer[256];
		std::cin.getline(Buffer,256,'}');
		system("cls");

		while(Update())
		{
			m_pInput->Poll();
			mouseXCoord += m_pInput->MouseX();
			mouseYCoord += m_pInput->MouseY();

			if(m_pInput->MouseClick(0))
			{
				POINT p;
				GetCursorPos (&p);
				wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",p.x,p.y);
			}

			m_pRendering->Begin();
			m_pRendering->DrawString(buffer);
			m_pRendering->End();
			m_pRendering->Present();
		}

		return 0;
	}

protected:

	IKMInput* m_pInput;
	IRenderingPlugin* m_pRendering;

	int mouseYCoord;
	int mouseXCoord;
	char buffer[64];

	void Load()
	{
		PluginManager& pluginManager = PluginManager::Instance();
		m_pInput  = static_cast<IKMInput*>(pluginManager.LoadDLL("DirectX Input DLL.dll"));
		m_pRendering = static_cast<IRenderingPlugin*>(pluginManager.LoadDLL("DX9 Rendering.dll"));
	}

	virtual ~InputTestApp() {}

};