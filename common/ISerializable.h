#ifndef _ISERIALIZABLE_
#define _ISERIALIZABLE_

#include <istream>
#include <ostream>

class ISerializable
{
public:

	virtual ~ISerializable() {}
	virtual bool Load(std::ifstream& stream) = 0;
	virtual bool Save(std::ofstream& stream) const = 0;
};

#endif // _ISERIALIZABLE_