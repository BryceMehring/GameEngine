#ifndef _SERIALIZEDSTATE_
#define _SERIALIZEDSTATE_

#include <map>
#include <string>

// Manages Loading and Saving of game state specific variables
class SerializedState
{
public:

	// Construct with file to associate with
	SerializedState(const std::string& file);

	virtual ~SerializedState() {}

	// Loads each line of the serialized state file into AngelScript
	virtual void Load(class asVM& vm);

	// Write values to save to the file to m_scriptFile
	// key = variable ID
	// data = value to be stored
	virtual void Save() = 0;

protected:

	std::map<std::string,std::string> m_scriptFile;
	const std::string& m_file;

	void BuildMap(const std::string& line);
	void SaveMap() const;

};

#endif //_SERIALIZEDSTATE_