#ifndef _FILEMANAGER_
#define _FILEMANAGER_

#include "Singleton.h"
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

class FileManager : public Singleton<FileManager>
{
public:

	friend class Singleton<FileManager>;

	~FileManager();

	void LoadAllFilesFromDictionary(std::vector<std::string>& files,
	const std::string& path, const std::string& ext) const;

	/*void LoadAllFilesFromDictionary(std::vector<std::string>& files,
	const boost::filesystem::path& path, const std::string& ext) const;*/

	std::string OpenFileName() const; 

	bool GetFolder(std::string& folderpath);

	unsigned int GetSeedFromLog() const;

	// replaced all of the overloaded versions of WriteToLog with
	// a template version
	template< class T >
	void WriteToLog(const T& data)
	{
		WriteTime();
		m_buffer << data << std::endl;
	}

	template< class T >
	bool ProccessFileByLine(const char* file, T& functor) const
	{
		std::fstream in(file,std::ios::in);

		if(in)
		{
			while(in)
			{
				std::string line;
				std::getline(in,line);

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
	FileManager();

	void WriteTime();
};

#endif

