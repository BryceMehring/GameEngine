

#include "FileManager.h"

using namespace std;

FileManager& FileManager::Instance()
{
	static FileManager instance;
	return instance;
}

void FileManager::WriteToLog(const std::string& str)
{
	WriteToLogGeneric(str);
}

void FileManager::WriteToLog(const char* str)
{
	WriteToLogGeneric(str);
}

void FileManager::WriteToLog(unsigned int i)
{
	WriteToLogGeneric(i);
}

void FileManager::WriteToLog(unsigned long i)
{
	WriteToLogGeneric(i);
}

void FileManager::WriteToLog(int i)
{
	WriteToLogGeneric(i);
}
void FileManager::WriteToLog(float f)
{
	WriteToLogGeneric(f);
}
void FileManager::WriteToLog(double d)
{
	WriteToLogGeneric(d);
}
void FileManager::WriteToLog(char c)
{
	WriteToLogGeneric(c);
}
void FileManager::WriteToLog(bool b)
{
	WriteToLogGeneric(b);
}

FileManager::FileManager()
{
	m_timer.Start();

	m_file.open("GameLog.txt");
	unsigned int counter = 2;
	while(!m_file.is_open())
	{
		std::stringstream stream;
		stream << counter++;

		m_file.open("GameLog_" + stream.str() + ".txt");
	}
}

FileManager::~FileManager()
{
	if(m_file.is_open())
	{
		m_file.close();
	}
}
