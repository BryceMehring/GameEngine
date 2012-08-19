#ifndef _GASSERT_
#define _GASSERT_

//#ifdef _DEBUG
#define gassert(condit,msg) \
	if(!(condit)) \
	{ \
		const std::string filename(__FILE__); \
		FileManager::Instance().WriteToLog(filename + " - " + msg); \
		MessageBox(0,msg,0,0); \
		PostQuitMessage(0); \
	}
/*#else
#define gassert(condit,msg) ((void)0)
#endif // _DEBUG*/

#endif //_GASSERT_