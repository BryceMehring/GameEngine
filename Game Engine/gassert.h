#ifndef _GASSERT_
#define _GASSERT_

// todo: fix this custom assert

//#ifdef _DEBUG
#define gassert(condit,msg) \
	if(!(condit)) \
	{ \
		const std::string filename(__FILE__); \
		FileManager::Instance().WriteToLog(filename + " - " + msg); \
	}
/*#else
#define gassert(condit,msg) ((void)0)
#endif // _DEBUG*/

#endif //_GASSERT_