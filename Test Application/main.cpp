
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
	PluginManager& pluginManager = PluginManager::Instance();

	// I want to put all of this Initialization into a dll
	BEngine::Initialize(hInstance,"Test");
	BEngine* pEngine = BEngine::GetInstance();

	IKMInput* pInput  = static_cast<IKMInput*>(pluginManager.LoadDLL("DirectX Input DLL.dll"));

	char buffer[64];
	ZeroMemory(buffer,32);
	int mouseYCoord = 0;
	int mouseXCoord = 0;

	while(pEngine->Update())
	{
		pInput->Poll();
		mouseXCoord += pInput->MouseX();
		mouseYCoord += pInput->MouseY();

		if(pInput->MouseClick(0))
		{
			wsprintf(buffer,"Mouse X: %d\nMouse Y:%d",mouseXCoord,mouseYCoord);
		}

		pEngine->DrawText(buffer);
		pEngine->Present();
	}

	BEngine::Delete();
	PluginManager::Delete();

	return 0;
}
				
																								  