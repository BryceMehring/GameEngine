#include "StringAlgorithms.h"
#include <algorithm>
#include <ctype.h>

void StringToLower(std::string& str)
{
	std::transform(str.begin(),str.end(),str.begin(),tolower);
}

void StringToUpper(std::string& str)
{
	std::transform(str.begin(),str.end(),str.begin(),toupper);
}

std::string GetFileNameFromPath(const std::string& file)
{
	// get the bounds of the actual filename
	size_t start = file.find_last_of("/\\") + 1;
	size_t n = file.find_last_of('.') - start;

	return file.substr(start,n);
}