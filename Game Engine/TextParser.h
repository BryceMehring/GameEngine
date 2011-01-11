#pragma once

using namespace std;

class TextParser
{
public:

	TextParser() {}
	~TextParser()
	{
	}

	void Parse(char* pFile)
	{
		m_inFile.open(pFile);

		if(m_inFile)
		{

		}
	}

private:

	fstream m_inFile;

};