#ifndef _DISTRIBUTEDFACTORY_
#define _DISTRIBUTEDFACTORY_

#include "Interfaces.h"

template< class C, class B = IEntity >
class Factory
{
public:
	
	typedef std::string KeyType;
	typedef typename C::TupleType TupleType;

	~Factory();

	B* CreateEntity(const KeyType& type,TupleType arg) const;
	bool RegisterType(const KeyType& type, C* pCreator);

private:

	typedef std::map<KeyType,C*> CreatorMap;
	CreatorMap m_creatorMap;

};

template< class C, class B >
Factory<C,B>::~Factory()
{
	for(CreatorMap::iterator iter = m_creatorMap.begin(); iter != m_creatorMap.end(); ++iter)
	{
		C* pCreator = iter->second;
		delete pCreator;
	}
}

template< class C, class B >
B* Factory<C,B>::CreateEntity(const KeyType& type,TupleType arg) const
{
	CreatorMap::const_iterator iter = m_creatorMap.find(type);

	if(iter == m_creatorMap.end()) { return nullptr; }

	C& creator = *iter->second;

	return creator.Create(arg);
}

template< class C, class B >
bool Factory<C,B>::RegisterType(const KeyType& type, C* pCreator)
{
#ifdef _DEBUG
	CreatorMap::iterator iter = m_creatorMap.find(type);
	if(iter != m_creatorMap.end())
	{
		delete pCreator;
		return false; 
	}
#endif

	m_creatorMap[type] = pCreator;
	return true;
}


#endif //_DISTRIBUTEDFACTORY_