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

#ifndef NULL
#define NULL 0
#endif

template< class T >
class Singleton
{
public:
	inline static T* Instance()
	{
		if(s_pInstance == NULL) 
		{ 
			s_pInstance = new T();
		}
		return s_pInstance;
	}

	static void Delete()
	{
		delete s_pInstance;
		s_pInstance = NULL;
	}

private:

	static T* s_pInstance;

};

template< class T >
T* Singleton<T>::s_pInstance = NULL;

#endif // _SINGLETON_