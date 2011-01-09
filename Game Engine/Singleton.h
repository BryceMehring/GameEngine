/*
	
	Bryce Mehring
	IS107

*/


// Singleton Macros
// Singleton's work by only allowing one instance of a class
// For the singleton to work, both the constructor and destructor must be private


#ifndef _SINGLETON_
#define _SINGLETON_
#pragma once

#ifndef NULL
#define NULL 0
#endif

#define DECLARE_SINGLETON(C) \
	private: \
		static C* s_pInstance; \
	public: \
		inline static C& Instance() \
		{ \
			if(!s_pInstance) \
			{ \
				s_pInstance = new C(); \
			} \
			return (*s_pInstance); \
		} \
		static void Delete(); \

#define IMPL_SINGLETON(C) \
	C* C::s_pInstance = NULL; \
	void C::Delete() \
	{ \
			delete s_pInstance;  \
			s_pInstance = NULL; \
	}

#endif