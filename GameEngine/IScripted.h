#ifndef _SCRIPTED_
#define _SCRIPTED_

#include <boost/thread/once.hpp>

template< class T >
class IScripted
{
public:

	static void RegisterScript(class asIScriptEngine* pEngine, IScripted* pThis = nullptr)
	{
		s_pScriptEngine = pEngine;
		s_pThis = pThis;

		boost::call_once(T::_RegisterScript,m_once);
	}

protected:

	// These data members only should be passed to _RegisterScript()
	static class asIScriptEngine* s_pScriptEngine; 
	static IScripted* s_pThis;

private:

	static boost::once_flag m_once;

};

template< class T >
asIScriptEngine* IScripted<T>::s_pScriptEngine = nullptr;

template< class T >
IScripted<T>* IScripted<T>::s_pThis = nullptr;

template< class T >
boost::once_flag IScripted<T>::m_once = BOOST_ONCE_INIT;

#endif 