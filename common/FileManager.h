#ifndef _FILEMANAGER_
#define _FILEMANAGER_

#include "Timer.h"
#include "CommonExport.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class FileManager
{
public:

	COMMON_API static FileManager& Instance();

	COMMON_API ~FileManager();
	
	// Write data to the log file with a timestamp
	COMMON_API void WriteToLog(const std::string&);
	COMMON_API void WriteToLog(const char*);
	COMMON_API void WriteToLog(unsigned int);
	COMMON_API void WriteToLog(unsigned long);
	COMMON_API void WriteToLog(int);
	COMMON_API void WriteToLog(float);
	COMMON_API void WriteToLog(double);
	COMMON_API void WriteToLog(char);
	COMMON_API void WriteToLog(bool);

private:

	std::ofstream m_file;
	Timer m_timer;

	template< class T >
	void WriteToLogGeneric(const T& data);

	FileManager();
};

template< class T >
void FileManager::WriteToLogGeneric(const T& data)
{
	std::stringstream stream;
	stream << m_timer.GetTime() << ": " << data;

	m_file << stream.str() << std::endl;
	std::cout << stream.str() << std::endl;
}

#endif

