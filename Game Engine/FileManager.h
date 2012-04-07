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

	std::string OpenFileName() const; 

	void WriteToLog(char c);
	void WriteToLog(int num);
	void WriteToLog(float num);
	void WriteToLog(const std::string& str);

private:

	std::ostringstream m_buffer;

	FileManager();
};

#endif

