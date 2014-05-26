#ifndef _COMMONEXPORT_
#define _COMMONEXPORT_

// Windows
#ifdef WIN32
// Link dynamic
	#ifdef COMMON_EXPORT
		#ifndef COMMON_API
		#define COMMON_API __declspec(dllexport)
		#endif
	#else
		#ifndef COMMON_API
		#define COMMON_API __declspec(dllimport) 
		#endif
	#endif
#else
#ifndef COMMON_API
#define COMMON_API
#endif
#endif

#endif // _COMMONEXPORT_