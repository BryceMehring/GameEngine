
#include "BEngine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	BEngine::Initialize(hInstance,"Hello World");

	BEngine* pEngine = BEngine::GetInstance();

	pEngine->LoadEffect(0,"Lighting.fx");

	pEngine->LoadXFile(0,0,"bigship1.x",[](ID3DXEffect* pEffect, const Mtrl& mtrl, DWORD i) 
	{
		switch(i)
		{
			case 0:

				pEffect->SetBool(

				break;

			case 1:

				break;

			default:

				break;
		}
	});

	while(pEngine->Update())
	{
		pEngine->Begin();

		pEngine->End();
		pEngine->Present();
	}

	BEngine::Delete();
	return 0;
}
                
                                                                                                  