
#include "stdafx.h"
#include "main.h"
#include "Factory.h"


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

	// registering 

	Factory<IUIElement>& uiFactory = Factory<IUIElement>::Instance();
	uiFactory.Register("CheckBox",new Creator<CheckBox,IUIElement>);
	//pUIInstance->Register("CheckBox",new Creator<CheckBox,IUIElement>());

	// UI
	m_pUI.reset(new UI());
	m_pUI->RegisterScript();

	// AS
	unsigned int id = m_pVM->BuildScriptFromFile("Test.as");
	m_pVM->ExecuteScript(id);
	m_pVM->ExecuteScript("as.ListFunctions()");
}
InputTestApp::~InputTestApp()
{
}

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


// The DLL files do not like the singletons. This is why when we pass a singleton
// instance into a dll, it appears null. I need to check this out more...

// the current fix gets rid of using a singleton and uses Reference Counting.
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	g_pEngine = new InputTestApp(hInstance,"AngelScript");

	int returnCode = g_pEngine->Run();

	delete g_pEngine;
	g_pEngine = NULL;

	return returnCode;
}
				
																								  