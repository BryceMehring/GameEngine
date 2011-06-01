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

template< class T >
class Singleton
{
public:
	
	// todo: I need to make this thread safe.
	static T& Instance()
	{
		static T instance;
		return instance;
	}

};

#endif // _SINGLETON_