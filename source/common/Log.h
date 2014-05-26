#ifndef _LOG_
#define _LOG_

#include "Timer.h"
#include "CommonExport.h"
#include <string>
#include <fstream>

class Log
{
public:

	COMMON_API static Log& Instance();
	
	// Write data to the log file with a timestamp
	COMMON_API void Write(const std::string&);
	COMMON_API void Write(const char*);
	COMMON_API void Write(unsigned int);
	COMMON_API void Write(unsigned long);
	COMMON_API void Write(int);
	COMMON_API void Write(float);
	COMMON_API void Write(double);
	COMMON_API void Write(char);
	COMMON_API void Write(bool);

private:

	std::ofstream m_file;
	Timer m_timer;

	Log();
	~Log();
};


#endif // _LOG_

