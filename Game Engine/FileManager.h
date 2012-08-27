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

	// todo: a lot of these methods could be global functions 

	friend class Singleton<FileManager>;

	~FileManager();

	//note: ext can be a list of extensions delimited by spaces
	// example: LoadAllFilesFromDictionary(... ,".h .cpp .txt .as")
	void LoadAllFilesFromDictionary(std::vector<std::string>& files,
	const std::string& path, const std::string& ext) const;

	std::string OpenFileName() const; 

	bool GetFolder(std::string& folderpath) const;

	unsigned int GetSeedFromLog() const;

	unsigned int CountNumberOfLines(const std::string& path, const std::string& ext) const;

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

	void GetDelimitedExtensions(const std::string& ext, std::vector<std::string>& out) const;
};

#endif

