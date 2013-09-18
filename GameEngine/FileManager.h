#ifndef _FILEMANAGER_
#define _FILEMANAGER_

#include "Singleton.h"
#include "Timer.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class FileManager : public Singleton<FileManager>
{
public:

	friend class Singleton<FileManager>;

	~FileManager();
	
	// Write data to the log file with a timestamp
	template< class T >
	void WriteToLog(const T& data)
	{
		std::stringstream stream;
		stream << m_timer.GetTime() << ": " << data;

		m_file << stream.str() << std::endl;
		std::cout << stream.str() << std::endl;
	}

	// Each line of the file gets passed to functor
	template< class T >
	bool ProccessFileByLine(const char* file, const T& functor) const
	{
		std::ifstream in(file);

		if(in.is_open())
		{
			std::string line;
			while(std::getline(in,line))
			{
				functor(line);
			}

			in.close();

			return true;
		}

		return false;
	}

	void RegisterScript(class asVM& vm);

private:

	std::ofstream m_file;
	Timer m_timer;

	FileManager();
};

#endif

