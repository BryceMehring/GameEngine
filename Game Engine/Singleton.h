/*
	
	Bryce Mehring
	IS107

*/


// Singleton Macros
// Singleton's work by only allowing one instance of a class
// For the singleton to work, both the constructor and destructor must be private
// todo: need to figure out how to get this file added to subversion.


#ifndef _SINGLETON_
#define _SINGLETON_
#pragma once

// Thread safe singleton
template< class T >
class Singleton
{
public:

	static T& Instance()
	{
		boost::call_once(once,Init);
		return *s_pInstance;
	}

private:

	// Init should never throw an exception
	static void Init()
	{
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