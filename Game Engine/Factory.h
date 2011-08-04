// Programmed by Bryce Mehring
// todo: need to add a description of the usage ...


#ifndef _FACTORY_
#define _FACTORY_
#pragma once

// abstract class
/*template< class T >
class ICreator
{
public:

	virtual ~ICreator() {}
	virtual T* Create(void* param) const = 0; // This function is pure virtual because..
};

// Creator class that allocates the interface to load a new instance of the class
template< class Product, class Base >
class Creator : public ICreator<Base>
{
public:

	virtual Base* Create(void* param) const { return new Product(param); }

};

// class T should be the base/parent class
template< class T >
class Factory : public Singleton<Factory<T>>
{
public:

	friend class Singleton<Factory<T>>;

	T* Create(const std::string& type, void* param = nullptr);
	void Register(const std::string& type, ICreator<T>* pCreator);

private:

	std::map<std::string,ICreator<T>*> m_CreatorMap;

};

template< class T >
T* Factory<T>::Create(const std::string& type, void* param)
{
	std::map<std::string,ICreator<T>*>::iterator iter = m_CreatorMap.find(type);

	// todo: an assert here is a bit too harsh. need to change it.
	assert(iter != m_CreatorMap.end());

	return iter->second->Create(param);
}

template< class T >
void Factory<T>::Register(const std::string& type, ICreator<T>* pCreator)
{
	m_CreatorMap[type] = pCreator;
}*/

#endif //_FACTORY_

