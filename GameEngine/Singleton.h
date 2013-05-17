/*
	Programmed by
	Bryce Mehring

*/


// Singleton's work by only allowing one instance of a class
// To use, inherit from this class and make the destructor private
// The child class must be friends with the Singleton<OBJECT_TYPE> in order for the singleton to destroy the child

#ifndef _SINGLETON_
#define _SINGLETON_

// Thread safe singleton
template< class T >
class Singleton
{
public:

	static T& Instance()
	{
		static T instance;
		return instance;
	}
};

#endif // _SINGLETON_
