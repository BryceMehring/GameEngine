#ifndef _SINGLEINSTACNE_
#define _SINGLEINSTACNE_

#include <Windows.h>

class SingleInstance
{
public:

	SingleInstance();
	~SingleInstance();

	int Run(HINSTANCE h,const char* pState);

private:

	HANDLE m_handle;

	void Error();
	int RunProgram(HINSTANCE h,const char* pState);

};

#endif //_SINGLEINSTACNE_