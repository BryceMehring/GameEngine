// Programmed by Bryce Mehring 

#ifndef _RTTI_
#define _RTTI_

#include <string>

// A fast and easy to debug RTTI system

#define RTTI_DECL \
	public: \
		static const RTTI s_rtti; \
		virtual const RTTI* GetType() const { return &s_rtti; }

#define RTTI_IMPL(name) \
	const RTTI name::s_rtti(#name);

// Each class will have a static member of this class
class RTTI
{
public:

	explicit RTTI(const std::string& className) : m_className(className) {}
	const std::string& GetName() const { return m_className; }

	bool IsEqual(const RTTI& other) const
	{
		return (this == &other);
	}

private:

	std::string m_className;

};

class IRTTI
{
public:

	virtual ~IRTTI() {}
	virtual const RTTI* GetType() const = 0;
};

#endif // _RTTI_