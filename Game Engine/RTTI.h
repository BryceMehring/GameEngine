#ifndef _RTTI_
#define _RTTI_

#include <string>

#define RTTI_DECL \
	public: \
		static const RTTI s_rtti; \
		virtual const RTTI* GetType() const { return &s_rtti; }

#define RTTI_IMPL(name) \
	const RTTI name::s_rtti(#name);

class RTTI
{
public:

	explicit RTTI(const std::string& className) : m_className(className) {}
	const std::string& GetName() const { return m_className; }

private:

	std::string m_className;

};

#endif // _RTTI_