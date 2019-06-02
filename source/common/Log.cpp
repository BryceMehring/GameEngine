

#include "Log.h"
#include <sstream>

Log& Log::Instance()
{
	static Log instance;
	return instance;
}

Log::Log()
{
	m_timer.Start();

	m_file.open("log.txt");

	if (!m_file.is_open()) 
	{
		throw std::runtime_error("Cannot open log.txt");
	}

	if (!m_file.is_open()) {
		Write("Cannot open log file");
	}
}

Log::~Log()
{
	if(m_file.is_open())
	{
		m_file.close();
	}
}


