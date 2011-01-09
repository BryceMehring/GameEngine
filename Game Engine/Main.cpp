
#include "StdAfx.h"
#include "BEngine.h"
#include "BCamera.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	BEngine::Initialize(hInstance,"Hello World");
	BEngine* pEngine = BEngine::GetInstance();

	/*pEngine->LoadEffect(0,"Lighting.fx");

	pEngine->LoadXFile(0,0,"bigship1.x");*/

	while(pEngine->Update())
	{
		pEngine->Begin();

		pEngine->End();
		pEngine->Present();
	}

	BEngine::Delete();
	return 0;
}
                
                                                                                                  