#ifndef _LOG_
#define _LOG_

#include "Timer.h"
#include "CommonExport.h"
#include <string>
#include <fstream>
#include <iostream>

class Log
{
public:

	COMMON_API static Log& Instance();
	
	template< class T >
	void Write(const T& data);

private:

	std::ofstream m_file;
	Timer m_timer;

	Log();
	Log(const Log&) = delete;
	Log& operator=(const Log&) = delete;
	~Log();
};

template< class T >
void Log::Write(const T& data)
{
	double time = m_timer.GetTime();
	std::cout << time << ": " << data << std::endl;
	m_file << time << ": " << data << std::endl;
}

#endif // _LOG_

