
#include "stdafx.h"
#include "EngineHelper.h"

using namespace std;

namespace EngineHelper
{

void LoadAllFilesFromDictionary(std::vector<std::string>& files, const boost::filesystem::path& path, const string& ext)
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

};