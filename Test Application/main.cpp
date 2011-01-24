
#include "BEngine.h"
#include "BCamera.h"
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
	BEngine::Initialize(hInstance,"Test");
	BEngine* pEngine = BEngine::GetInstance();

	PluginManager* pPluginMgr = new PluginManager();

	IInputPlugin* pInput  = static_cast<IInputPlugin*>(pPluginMgr->LoadDLL("DirectX Input DLL.dll"));

	pInput->Poll();
	int mouseXCoord = pInput->MouseX();

	while(pEngine->Update())
	{
		pEngine->Present();
	}

	BEngine::Delete();

	return 0;
}
                
                                                                                                  