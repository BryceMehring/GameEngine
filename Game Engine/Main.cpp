
#include "BEngine.h"
#include <functional>

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	BEngine::Initialize(hInstance,"Hello World");

	BEngine* pEngine = BEngine::GetInstance();

	pEngine->LoadEffect(0,"Lighting.fx");

	auto test = [](ID3DXEffect* pEffect, const Mtrl& mtrl, DWORD i) -> void
	{
		pEffect->SetTechnique("PhongLighting");
		pEffect->SetValue("gObjectMtrl",(void*)&mtrl.mtrl,sizeof(D3DMATERIAL9));

		switch(i)
		{
			case 0:				
				break;
			case 1:
				break;
			default:
				break;
		}
	};

	// need to get the transformation matrix into the lambda expression
	pEngine->LoadXFile(0,0,"bigship1.x",test);


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
                
                                                                                                  