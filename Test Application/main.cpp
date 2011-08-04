
#include "stdafx.h"
#include "main.h"
#include "Factory.h"
#include "EngineHelper.h"
#include "UI.h"
#include "DxPolygon.h"

#pragma comment(lib,"Game Engine.lib")
#pragma comment(lib,"d3dx9.lib")

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


InputTestApp::InputTestApp(HINSTANCE hInstance,const string& winCaption) : BEngine(hInstance,winCaption)
{
	ZeroMemory(buffer,sizeof(buffer));

	LoadAndExecScripts();
}
InputTestApp::~InputTestApp()
{
}

int InputTestApp::Run()
{
	static float time = 0.0f;
	
	while(Update())
	{
		StartCounter();

		m_pUI->Update(m_fDT);

		// do game update here
		m_pRenderer->Begin();

		m_pUI->Render(*m_pRenderer);
			
		//if(m_pInput->MouseClick(0))
		{
			POINT p;
			m_pInput->MousePos(p);

			RECT R = {650,0,0,0};
			clock_t c = clock() / CLOCKS_PER_SEC ;
			wsprintf(buffer,"Time: %d",c);
			wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",p.x,p.y);
			m_pRenderer->DrawString(buffer,R,0xffBfffff);
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
	EngineHelper::LoadAllFilesFromDictionary(files,"..\\Scripts\\",".as");

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
	boost::scoped_ptr<BEngine> pEngine(new InputTestApp(hInstance,"Game Engine"));
	return pEngine->Run();
}
				
																								  