// programmed by Bryce Mehring
// Start Date: 8/20/2011
// Last edit: Spring 2013

// Note: These delegates only support 1 argument
// todo: I could add support for n amount of arguments with Variadic templates when MSVS updates to the new
// C++0x standard.

/*

Delegates are a C++ way of representing function pointers, but are a lot more powerful. They can point to global functions, member functions
and even script functions

Example:

void Function(int a)
{
	cout<<a<<endl;
}

Delegate<void,int> myDelegate;
myDelegate.Bind(Function);
myDelegate.Call(5); // prints 5

*/


#ifndef _DELEGATES_
#define _DELEGATES_

#include "RefCounting.h"
#include <map>

// Base Interface for Delegates
template< class RETURN, class PARAM>
class IFunction : public RefCounting
{
public:

	virtual RETURN Call(PARAM) const = 0;

protected:

	virtual ~IFunction() {}

};

template< class RETURN >
class IFunction<RETURN,void> : public RefCounting
{
public:

	virtual RETURN Call() const = 0;

protected:

	virtual ~IFunction() {}

};

// GlobalFunction version of IFunction
template< class RETURN, class PARAM>
class GlobalFunction : public IFunction<RETURN,PARAM>
{
public:

	// global function signature
	typedef RETURN (*PTR)(PARAM);

	GlobalFunction(PTR function) : m_function(function) {}

	virtual RETURN Call(PARAM param) const
	{
		return (*m_function)(param);
	}

private:

	PTR m_function;

};

template< class RETURN >
class GlobalFunction<RETURN,void> : public IFunction<RETURN,void>
{
public:

	typedef RETURN (*PTR)();

	GlobalFunction(PTR function) : m_function(function) {}

	virtual RETURN Call() const
	{
		return (*m_function)();
	}

private:

	PTR m_function;

};

// MemberFunction version of IFunction
template<class CLASS, class RETURN, class PARAM>
class MemberFunction : public IFunction<RETURN,PARAM>
{
public:

	// class method signature
	typedef RETURN (CLASS::*PTR)(PARAM);

	MemberFunction(CLASS* pThis,PTR method) : m_pThis(pThis), m_method(method) {}

	virtual RETURN Call(PARAM param) const
	{
		return (m_pThis->*m_method)(param);
	}

private:

	CLASS* m_pThis;
	PTR m_method;

};

template<class CLASS, class RETURN>
class MemberFunction<CLASS,RETURN,void> : public IFunction<RETURN,void>
{
public:

	typedef RETURN (CLASS::*PTR)();

	MemberFunction(CLASS* pThis,PTR method) : m_pThis(pThis), m_method(method) {}

	virtual RETURN Call() const
	{
		return (m_pThis->*m_method)();
	}

private:

	CLASS* m_pThis;
	PTR m_method;

};

// ConstMemberFunction version of IFunction
template<class CLASS, class RETURN, class PARAM>
class ConstMemberFunction : public IFunction<RETURN,PARAM>
{
public:

	// class method signature
	typedef RETURN (CLASS::*PTR)(PARAM) const;

	ConstMemberFunction(const CLASS* pThis,PTR method) : m_pThis(pThis), m_method(method) {}

	virtual RETURN Call(PARAM param) const
	{
		return (m_pThis->*m_method)(param);
	}

private:

	const CLASS* m_pThis;
	PTR m_method;

};

template<class CLASS, class RETURN>
class ConstMemberFunction<CLASS,RETURN,void> : public IFunction<RETURN,void>
{
public:

	typedef RETURN (CLASS::*PTR)() const;

	ConstMemberFunction(const CLASS* pThis,PTR method) : m_pThis(pThis), m_method(method) {}

	virtual RETURN Call() const
	{
		return (m_pThis->*m_method)();
	}

private:

	const CLASS* m_pThis;
	PTR m_method;

};


// The Base Delegate class
// This class has a IFunction object which manages binding

// common functionality of delegates

template< class RETURN, class PARAM>
class DelegateBase
{
public:

	DelegateBase() : m_ptr(nullptr) {}
	~DelegateBase()
	{
		Unbind();
	}

	DelegateBase(const DelegateBase& d)
	{
		CopyConstruct(d);
	}

	void Unbind()
	{
		if(IsBound())
		{
			m_ptr->Release();
			m_ptr = nullptr;
		}
	}

	bool IsBound() const { return m_ptr != nullptr; }

	DelegateBase& operator =(const DelegateBase& d)
	{
		if(this != &d)
		{
			// Release old pointer
			Unbind();

			// copy the new pointer
			CopyConstruct(d);
		}

		return *this;
	}

protected:

	IFunction<RETURN,PARAM>* m_ptr;

	void CopyConstruct(const DelegateBase& d)
	{
		m_ptr = d.m_ptr;
		if(IsBound())
		{
			m_ptr->AddRef();
		}
	}

};


// Delegate with parameter and return
template< class RETURN, class PARAM>
class Delegate : public DelegateBase<RETURN,PARAM>
{
public:

	using DelegateBase<RETURN,PARAM>::m_ptr;

	Delegate() {}
	Delegate(typename GlobalFunction<RETURN,PARAM>::PTR pFunc)
	{
		Bind(pFunc);
	}

	template< class T >
	Delegate(T* pThis, RETURN (T::*pFunc)(PARAM))
	{
		Bind(pThis,pFunc);
	}

	template< class T >
	Delegate(const T* pThis, RETURN (T::*pFunc)(PARAM) const)
	{
		Bind(pThis,pFunc);
	}

	void Bind(typename GlobalFunction<RETURN,PARAM>::PTR pFunc)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new GlobalFunction<RETURN,PARAM>(pFunc);
		}
	}

	template< class T >
	void Bind(T* pThis, RETURN (T::*pFunc)(PARAM))
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new MemberFunction<T,RETURN,PARAM>(pThis,pFunc);
		}
	}

	template< class T >
	void Bind(const T* pThis, RETURN (T::*pFunc)(PARAM) const)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new ConstMemberFunction<T,RETURN,PARAM>(pThis,pFunc);
		}
	}

	RETURN Call(PARAM param) const
	{
		return this->m_ptr->Call(param);
	}

};

// Delegate without parameters and return
template< class RETURN >
class Delegate<RETURN,void> : public DelegateBase<RETURN,void>
{
public:

	using DelegateBase<RETURN,void>::m_ptr;

	Delegate() {}
	Delegate(typename GlobalFunction<RETURN,void>::PTR pFunc)
	{
		Bind(pFunc);
	}

	template< class T >
	Delegate(T* pThis, RETURN (T::*pFunc)())
	{
		Bind(pThis,pFunc);
	}

	template< class T >
	Delegate(const T* pThis, RETURN (T::*pFunc)() const)
	{
		Bind(pThis,pFunc);
	}

	void Bind(typename GlobalFunction<RETURN,void>::PTR pFunc)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new GlobalFunction<RETURN,void>(pFunc);
		}
	}

	template< class T >
	void Bind(T* pThis, RETURN (T::*pFunc)())
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new MemberFunction<T,RETURN,void>(pThis,pFunc);
		}
	}

	template< class T >
	void Bind(const T* pThis, RETURN (T::*pFunc)() const)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new ConstMemberFunction<T,RETURN,void>(pThis,pFunc);
		}
	}

	RETURN Call() const
	{
		return this->m_ptr->Call();
	}
};


// these event class ignores the return value

// common event functionality
template < class RETURN , class PARAM >
class EventBase
{
public:

	typedef Delegate<RETURN,PARAM> DELEGATE;

	EventBase() : m_iId(0) {}

	// adds a Delegate to the map and returns its key.
	int Attach(const DELEGATE& d)
	{
		unsigned int id = m_iId;
		auto iter = m_delegates.find(m_iId);

		while(iter != m_delegates.end())
		{
			id = ++m_iId;
			iter = m_delegates.find(m_iId);
		}

		m_delegates.insert(std::make_pair(m_iId++,d));

		return id;
	}
	// Detaches a delegate by its key
	void Detach(int id)
	{
		m_delegates.erase(id);
	}

protected:

	typedef std::map<int,DELEGATE> DelegateMap;

	DelegateMap m_delegates;
	int m_iId;
};

// Event with parameters
template < class RETURN , class PARAM >
class Event : public EventBase<RETURN,PARAM>
{
public:

	using EventBase<RETURN,PARAM>::m_delegates;

	// Notify all delegates
	void Notify(PARAM a)
	{
		for(auto iter = m_delegates.begin(); iter != m_delegates.end(); iter++)
		{
			iter->second.Call(a);
		}
	}
};

// Event without parameters
template < class RETURN >
class Event<RETURN,void> : public EventBase<RETURN,void>
{
public:

	using EventBase<RETURN,void>::m_delegates;

	void Notify()
	{
		for(auto iter = m_delegates.begin(); iter != m_delegates.end(); iter++)
		{
			iter->second.Call();
		}
	}
};

#endif // _DELEGATES_

