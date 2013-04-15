#ifndef _SERIALIZEDSTATE_
#define _SERIALIZEDSTATE_

#include <map>
#include <string>

class SerializedState
{
public:

	SerializedState(const char* pFile);

	virtual ~SerializedState() {}
	virtual void Load(class asVM& vm);
	virtual void Save() = 0;

protected:

	std::map<std::string,std::string> m_scriptFile;
	const char* m_pFile;

	void BuildMap(const std::string& line);
	void SaveMap() const;

};

#endif //_SERIALIZEDSTATE_