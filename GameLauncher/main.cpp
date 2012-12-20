#include "SingleInstance.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	SingleInstance instance;
	return instance.Run(hInstance,lpCmdLine);
}
