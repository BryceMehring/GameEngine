

#include "FileManager.h"
#include "asVM.h"
#include "GameConstants.h"
#include "StringAlgorithms.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <Shlobj.h>

using namespace std;

FileManager::~FileManager()
{
	::fstream out(Constants::LOGFILE,ios::out);

	if(out.is_open())
	{
		out << m_buffer.str() << endl;
		out.close();
	}
}


unsigned int FileManager::CountNumberOfLines(const string& path, const string& ext) const
{
	std::vector<std::string> files;
	LoadAllFilesFromDirectory(files,path,ext);

	unsigned int uiTotalLines = 0;
	for(unsigned int i = 0; i < files.size(); ++i)
	{
		std::ifstream inFile(files[i]); 
		uiTotalLines += std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
	}

	return uiTotalLines;
}


void FileManager::LoadAllFilesFromDirectory(std::vector<std::string>& files, const std::string& path, const std::string& ext) const
{
	// If there is a path and ext
	if(!path.empty() && !ext.empty())
	{
		// extract all of the extensions from the string
		std::vector<string> extensions;
		GetDelimitedString(ext,extensions);

		// stack emulates recursion
		std::stack<std::string> dir;
		dir.push(path);

		// Loop over all of the dictionaries
		while(!dir.empty())
		{
			string top = dir.top();
			dir.pop();

			// Loop over all files/dict in path
			for ( boost::filesystem::directory_iterator it( top ); it != boost::filesystem::directory_iterator(); ++it )
			{
				// If the current path is a directory
				if(boost::filesystem::is_directory(it->status()))
				{
					// Add it to the stack
					dir.push(it->path().string());
				}
				// If the current path is a file
				else if ( boost::filesystem::is_regular_file( it->status() ))
				{
					// Loop over all of the extensions
					for(unsigned int i = 0; i < extensions.size(); ++i)
					{
						// if the file extension matches one in the list of extensions
						if( boost::algorithm::to_lower_copy(it->path().extension().string()) == extensions[i] )
						{
							// add the file to the vector
							files.push_back(it->path().string());
							break;
						}
					}
				}

			}

		}
	}

}

bool FileManager::GetFolder(std::string& folderpath) const
{
   bool retVal = false;

   // The BROWSEINFO struct tells the shell 
   // how it should display the dialog.
   BROWSEINFO bi;
   memset(&bi, 0, sizeof(bi));

   bi.ulFlags   = BIF_USENEWUI;

   // must call this if using BIF_USENEWUI
   ::OleInitialize(NULL);

   // Show the dialog and get the itemIDList for the 
   // selected folder.
   LPITEMIDLIST pIDL = ::SHBrowseForFolder(&bi);

   if(pIDL != NULL)
   {
      // Create a buffer to store the path, then 
      // get the path.
      char buffer[_MAX_PATH] = {'\0'};
      if(::SHGetPathFromIDList(pIDL, buffer) != 0)
      {
         // Set the string value.
         folderpath = buffer;
         retVal = true;
      }

      // free the item id list
      CoTaskMemFree(pIDL);
   }

   ::OleUninitialize();

   return retVal;
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

	if(GetOpenFileName(&ofn) > 0)
	{
		return std::string(fileName);
	}

	return std::string();
}

unsigned int FileManager::GetSeedFromLog() const
{
	unsigned int seed = 0;
	std::fstream in(::Constants::LOGFILE,ios::in);

	if(in.is_open())
	{
		string line;
		getline(in,line);

		unsigned int pos = line.find_last_of(':');

		if(pos != string::npos)
		{
			stringstream oss(line.substr(pos+1));
			oss >> seed;
		}

		in.close();
	}

	seed += (unsigned int)time(NULL);

	return seed;
}

void FileManager::WriteTime()
{
	m_buffer << clock() / (float)CLOCKS_PER_SEC << ": ";
}

void FileManager::RegisterScript(asVM& vm)
{
	auto pEngine = vm.GetScriptEngine();

	const char* NAME = "FileManager";

	DBAS(pEngine->RegisterObjectType(NAME,0,asOBJ_REF | asOBJ_NOHANDLE));

	//DBAS(pEngine->RegisterObjectMethod(NAME,"void LoadAllFilesFromDictionary(array<string>& out, const string& in, const string& in) const",asMETHOD(FileManager,LoadAllFilesFromDictionary),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod(NAME,"uint CountNumberOfLines(const string& in, const string& in) const",asMETHOD(FileManager,CountNumberOfLines),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod(NAME,"string OpenFileName() const",asMETHOD(FileManager,OpenFileName),asCALL_THISCALL));
	DBAS(pEngine->RegisterObjectMethod(NAME,"bool GetFolder(string& out) const",asMETHOD(FileManager,GetFolder),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("FileManager fm",this));

	pEngine->Release();
}