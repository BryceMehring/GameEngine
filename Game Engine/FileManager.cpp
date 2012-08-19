

#include "FileManager.h"
#include "asVM.h"
#include "GameConstants.h"
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <Shlobj.h>

using namespace std;

FileManager::FileManager() {}
FileManager::~FileManager()
{
	::fstream out(Constants::LOGFILE,ios::out);

	if(out.is_open())
	{
		out << m_buffer.str() << endl;
		out.close();
	}
}
void FileManager::LoadAllFilesFromDictionary(std::vector<std::string>& files, const std::string& path, const std::string& ext) const
{
	// note: when the path is empty, it causes the program to crash
	if(!path.empty())
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

}

bool FileManager::GetFolder(std::string& folderpath)
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

	DBAS(pEngine->RegisterObjectMethod(NAME,"string OpenFileName() const",asMETHOD(FileManager,OpenFileName),asCALL_THISCALL));
	DBAS(pEngine->RegisterGlobalProperty("FileManager fm",this));

	pEngine->Release();
}