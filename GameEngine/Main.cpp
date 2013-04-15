
#include "StdAfx.h"
#include "BEngine.h"
#include "BCamera.h"
#include "BFont.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	BEngine::Initialize(hInstance,"Test");
	BEngine* pEngine = BEngine::GetInstance();

	while(pEngine->Update())
	{
		pEngine->Present();
	}

	BEngine::Delete();

	return 0;
}
                
                                                                                                  