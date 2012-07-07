/*
	Programmed by
	Bryce Mehring

*/


// Singleton's work by only allowing one instance of a class

#ifndef _SINGLETON_
#define _SINGLETON_
#pragma once

#include <boost\scoped_ptr.hpp>
#include <boost\thread\once.hpp>


// Thread safe singleton
template< class T >
class Singleton
{
public:

	// note: using boost here fails when creating the global creator objects
	// todo: fix this
	static T& Instance()
	{
		static T instance;
		return instance;
		//boost::call_once(once,Init);
		//return *s_pInstance;
	}

private:

	//note: Init should never throw an exception
	static void Init() throw()
	{
		// the constructor should never call this same function again, or else it would fail 
		s_pInstance.reset(new T());
	}

	static boost::scoped_ptr<T> s_pInstance;
	static boost::once_flag once;
};

template< class T >
boost::scoped_ptr<T> Singleton<T>::s_pInstance;

template< class T >
boost::once_flag Singleton<T>::once = BOOST_ONCE_INIT;

#endif // _SINGLETON_