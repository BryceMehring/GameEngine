#ifndef _FILEMANAGER_
#define _FILEMANAGER_

#include "Singleton.h"
#include <vector>
#include <string>
#include <sstream>

class FileManager : public Singleton<FileManager>
{
public:

	friend class Singleton<FileManager>;

	~FileManager();

	void LoadAllFilesFromDictionary(std::vector<std::string>& files,
	const std::string& path, const std::string& ext) const;

	/*void LoadAllFilesFromDictionary(std::vector<std::string>& files,
	const boost::filesystem::path& path, const std::string& ext) const;*/

	bool OpenFileName(std::string& file) const; 

	bool GetFolder(std::string& folderpath);

	// replaced all of the overloaded versions of WriteToLog with
	// a template version
	template< class T >
	void WriteToLog(const T& data)
	{
		WriteTime();
		m_buffer << data << std::endl;
	}

private:

	std::ostringstream m_buffer;

	// constructor
	FileManager();

	void WriteTime();
};

#endif

