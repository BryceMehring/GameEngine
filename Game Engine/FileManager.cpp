

#include "FileManager.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <Windows.h>
#include <fstream>

using namespace std;


FileManager::FileManager() {}
FileManager::~FileManager()
{
	::fstream out("..//Log.txt",ios::out);

	if(out.is_open())
	{
		out << m_buffer.str() << endl;
		out.close();
	}
}
void FileManager::LoadAllFilesFromDictionary(std::vector<std::string>& files, const std::string& path, const std::string& ext) const
{
	for ( boost::filesystem::directory_iterator it( path );
        it != boost::filesystem::directory_iterator(); ++it )
	{
		if ( boost::filesystem::is_regular_file( it->status() ))
		{
			if( boost::algorithm::to_lower_copy(it->path().extension().string()) == ext)
			{
				files.push_back(it->path().string());
			}

		}

	}

}

std::string FileManager::OpenFileName() const
{
	OPENFILENAME ofn = {0};
	char fileName[MAX_PATH] = "";
 
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = 0;
	ofn.lpstrFilter = "All Files (*.*)\0*.*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
 
	GetOpenFileName(&ofn);
	return std::string(fileName);
}

void FileManager::WriteToLog(const std::string& str)
{
	m_buffer << str.c_str() << endl;
}

void FileManager::WriteToLog(float num)
{
	m_buffer << num << endl;
}

void FileManager::WriteToLog(int num)
{
	m_buffer << num << endl;
}

void FileManager::WriteToLog(char c)
{
	m_buffer << c << endl;
}