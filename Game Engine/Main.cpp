
#include "BEngine.h"
#include "BCamera.h"

using namespace std;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	BEngine::Initialize(hInstance,"Hello World");
	//BCamera::Initialize();

	BEngine* pEngine = BEngine::GetInstance();

	pEngine->LoadEffect(0,"Lighting.fx");

	pEngine->LoadXFile(0,0,"bigship1.x");

	while(pEngine->Update())
	{
		pEngine->Begin();

		pEngine->RenderMesh(0);

		pEngine->End();
		pEngine->Present();
	}

	BEngine::Delete();
	return 0;
}
                
                                                                                                  