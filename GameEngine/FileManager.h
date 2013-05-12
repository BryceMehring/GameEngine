#ifndef _FILEMANAGER_
#define _FILEMANAGER_

#include "Singleton.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <ctime>

class FileManager : public Singleton<FileManager>
{
public:

	// todo: a lot of these methods could be global functions 

	friend class Singleton<FileManager>;

	~FileManager();
	
	bool GetFolder(std::string& folderpath) const;

	// replaced all of the overloaded versions of WriteToLog with
	// a template version
	template< class T >
	void WriteToLog(const T& data)
	{
        m_buffer << clock() / 1000.0f << ' ' << data << std::endl;
	}

	template< class T >
    bool ProccessFileByLine(const char* file,const T& functor) const
	{
		std::fstream in(file,std::ios::in);

		if(in)
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

	std::ostringstream m_buffer;

	// constructor
    FileManager() {}

	void WriteTime();
};

#endif

