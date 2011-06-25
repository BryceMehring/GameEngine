
#include "stdafx.h"
#include "main.h"
#include "Factory.h"
#include "EngineHelper.h"
#include "UI.h"

#pragma comment(lib,"Game Engine.lib")

using namespace AngelScript;
using namespace std;

// And then, as we go up in recursion, we display the rest of the numbers
/*template< int n >
struct LoopClass
{
	static void Loop()
	{
		LoopClass<n-1>::Loop();
		cout<<n<<" ";
	}
};

// This Loop gets called first
template<>
struct LoopClass<1>
{
	static void Loop()
	{
		cout<<"1 ";
	}
};*/


InputTestApp::InputTestApp(HINSTANCE hInstance,const string& winCaption) : IBaseEngine(hInstance,winCaption)
{
	// todo: I need to move initialization into smaller helper function

	ZeroMemory(buffer,sizeof(buffer));

	LoadAndExecScripts();
}
InputTestApp::~InputTestApp()
{
}

/*void InputTestApp::MsgProc(UINT msg, WPARAM wPraram, LPARAM lparam)
{
	// todo: is this the best location for this code?
	// todo: I need to be able to turn this on and off
	switch(msg)
	{
	case WM_CHAR:
		{
			if(wPraram == 8)
			{
				if(!m_str.empty())
				{
					m_str.resize(m_str.size() - 1);
				}
			}
			else if(wPraram == 13)
			{
				m_str += "\n";
				//m_vm->ExecuteScript(m_str.c_str());
			//	m_str.clear();
			}
			else
			{
				m_str += char(wPraram);
			}

		}
		
		break;
	}

	m_pUI->AddChar(char(wPraram));

	return IBaseEngine::MsgProc(msg,wPraram,lparam);
}*/

int InputTestApp::Run()
{
	while(Update())
	{
		StartCounter();
		m_pInput->Poll();

		m_pUI->Update(m_fDT);

		// do game update here
	
		m_pRenderer->Begin();

		m_pUI->Render();
			
		//pUI->Render();
		//if(m_pInput->MouseClick(0))
		{
			POINT p;
			m_pInput->MousePos(p);

			RECT R = {250,50,0,0};
			clock_t c = clock() / CLOCKS_PER_SEC ;
			wsprintf(buffer,"Time: %d",c);
			//wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",p.x,p.y);
			m_pRenderer->DrawString(buffer,R,0xffffffff);
		}
		m_pRenderer->End();
		m_pRenderer->Present();
		EndCounter();
	}

	return 0;
}

void InputTestApp::LoadAndExecScripts()
{
	// AS
	vector<string> files;
	EngineHelper::LoadAllFilesFromDictionary(files,"../Scripts/",".as");

	for(unsigned int i = 0; i < files.size(); ++i)
	{
		unsigned int index = m_vm->BuildScriptFromFile(files[i]);
		m_vm->ExecuteScript(index);
	}
}
 


// The DLL files do not like the singletons. This is why when we pass a singleton
// instance into a dll, it appears null. I need to check this out more...

// the current fix gets rid of using a singleton and uses Reference Counting.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	boost::scoped_ptr<IBaseEngine> pEngine(new InputTestApp(hInstance,"AngelScript"));
	return pEngine->Run();
}
				
																								  