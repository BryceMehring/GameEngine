#ifndef _STRINGALGORITHMS_
#define _STRINGALGORITHMS_

#include <string>

// todo: wrap into a namespace

void StringToLower(std::string& str);
void StringToUpper(std::string& str);

std::string GetFileNameFromPath(const std::string& file);

std::string WrapWithQuote(const std::string& str);

float CompareFiles(const std::string& file1, const std::string& file2);

void RegisterStrAlgorScript(class asVM&);

#endif //_STRINGALGORITHMS_