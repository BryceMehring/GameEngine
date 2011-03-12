
#include "main.h"


// The DLL files do not like the singletons. This is why when we pass a singleton
// instance into a dll, it appears null. I need to check this out more...

// the current fix gets rid of using a singleton and uses Reference Counting.

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	// I want to put all of this Initialization into a dll
	g_pEngine = new InputTestApp(hInstance,"Test");

	int returnCode = g_pEngine->Run();

	PluginManager::Delete();
	g_pEngine->Release();

	return returnCode;
}
				
																								  