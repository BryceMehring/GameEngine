#include "SingleInstance.h"

#pragma comment(lib,"Game Engine.lib")

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	SingleInstance instance;
	return instance.Run(hInstance,lpCmdLine);
}
