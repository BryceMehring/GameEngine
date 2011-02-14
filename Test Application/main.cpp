
#include "BEngine.h"
#include "BFont.h"

#include "PluginManager.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

using namespace std;

// The DLL files do not like the singletons. This is why when we pass a singleton
// instance into a dll, it appears null. I need to check this out more...

// the current fix gets rid of using a singleton and uses Reference Counting.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	g_pEngine = new BEngine(hInstance,"Test");
	PluginManager* pPluginMgr = new PluginManager();

	IInputPlugin* pInput  = static_cast<IInputPlugin*>(pPluginMgr->LoadDLL("DirectX Input DLL.dll"));
	pInput->About();
	
	char buffer[32];
	ZeroMemory(buffer,32);
	int mouseXCoord = 0;
	int mouseYCoord = 0;

	while(g_pEngine->Update())
	{
		pInput->Poll();
		mouseXCoord += pInput->MouseX();
		mouseYCoord += pInput->MouseY();

		if(pInput->MouseClick(0))
		{
			wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",mouseXCoord,mouseYCoord);
		}

		g_pEngine->DrawText(buffer);
		g_pEngine->Present();
	}

	g_pEngine->Release();
	pPluginMgr->Release();

	return 0;
}
                
                                                                                                  