#ifndef _SINGLEINSTACNE_
#define _SINGLEINSTACNE_

#include <Windows.h>

// This class will only create one instance of the program, even if the program has been launched a 2nd time.
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