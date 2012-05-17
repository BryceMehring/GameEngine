// programmed by Bryce Mehring
// Start Date: 8/20/2011
// Last edit: 9/10/2011

// Note: These delegates only support 1 argument 
// todo: I could add support for n amount of arguments with Variadic templates when MSVS updates to the new
// C++0x standard.


#ifndef _DELEGATES_
#define _DELEGATES_

#include "RefCounting.h"
#include "asVM.h"
#include <hash_map>

// Base Interface for Delegates
template< class RETURN, class PARAM>
class IFunction : public RefCounting
{
public:

	virtual ~IFunction() {}
	virtual RETURN Call(PARAM) const = 0;

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

class ScriptFunction : public IFunction<void,void>
{
public:

	ScriptFunction(asVM* pVM,const ScriptFunctionStruct& func) : m_pVM(pVM), m_func(func) {}

	virtual void Call() const
	{
		m_pVM->ExecuteScriptFunction(m_func);
	}

private:

	asVM* m_pVM;
	ScriptFunctionStruct m_func;

};

// The Base Delegate class
// This class has a IFunction object which manages binding

// common functionality of delegates

template< class RETURN, class PARAM>
class DelegateBase
{
public:

	typedef IFunction<RETURN,PARAM> GENERIC_FUNC;

	DelegateBase() : m_ptr(nullptr) {}
	DelegateBase(typename GlobalFunction<RETURN,PARAM>::PTR pFunc) : m_ptr(nullptr)
	{
		Bind(pFunc);
	}


	template< class T >
	DelegateBase(T* pThis,RETURN (T::*pFunc)(PARAM)) : m_ptr(nullptr)
	{
		Bind(pThis,pFunc);
	}
	
	template< class T >
	DelegateBase(const T* pThis,RETURN (T::*pFunc)(PARAM) const) : m_ptr(nullptr)
	{
		Bind(pThis,pFunc);
	}

	DelegateBase(const DelegateBase& d)
	{
		m_ptr = d.m_ptr;
		if(m_ptr)
		{
			m_ptr->AddRef();
		}
	}
	~DelegateBase()
	{
		Unbind();
	}

	void Bind(typename GlobalFunction<RETURN,PARAM>::PTR pFunc)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new GlobalFunction<RETURN,PARAM>(pFunc);
		}
	}

	void Bind(asVM* pm, const ScriptFunctionStruct& func)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new ScriptFunction(pm,func);
		}
	}

	/*template< class CLASS >
	void Bind(const CLASS* pThis,typename const ConstMemberFunctionData<CLASS,RETURN,PARAM>::PTR pFunc)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new ConstMemberFunctionData<CLASS,RETURN,PARAM>(pThis,pFunc);
			//m_ptr = TestFunct<CLASS>::GetFunct(pThis,pFunc);
			//m_ptr = new ConstMemberFunction<CLASS,RETURN,PARAM>((pThis),pFunc);
		}
	}*/

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

	/*template< class CLASS >
	void Bind(CLASS* pThis,typename ConstMemberFunction<CLASS,RETURN,PARAM>::PTR pFunc,int)
	{
		if(m_ptr == nullptr)
		{
			m_ptr = new ConstMemberFunctionData<CLASS,RETURN,PARAM>(pThis,pFunc);
		}
	}*/

	void Unbind()
	{
		if(m_ptr)
		{
			m_ptr->Release();
			m_ptr = nullptr;
		}
	}

	bool IsBound() { return m_ptr != nullptr; }

	DelegateBase& operator =(const DelegateBase& d)
	{
		if(this != &d)
		{
			// Release old pointer
			if(m_ptr)
			{
				m_ptr->Release();
			}

			// copy the new pointer
			m_ptr = d.m_ptr;

			// Keep a copy, call AddRef
			if(m_ptr)
			{
				m_ptr->AddRef();
			}
		}

		return *this;
	}

protected:

	GENERIC_FUNC* m_ptr;

};


// Delegate with parameter
template< class RETURN, class PARAM>
class Delegate : public DelegateBase<RETURN,PARAM>
{
public:

	Delegate() {}
	Delegate(typename GlobalFunction<RETURN,PARAM>::PTR pFunc) : DelegateBase(pFunc) {}

	template< class T >
	Delegate(T* pThis,RETURN (T::*pFunc)(PARAM)) : DelegateBase(pThis,pFunc)
	{	
	}
	
	template< class T >
	Delegate(const T* pThis,RETURN (T::*pFunc)(PARAM) const) : DelegateBase(pThis,pFunc)
	{
	}

	RETURN Call(PARAM param) const
	{
		return m_ptr->Call(param);
	}

};

// Delegate without parameters
template< class RETURN >
class Delegate<RETURN,void> : public DelegateBase<RETURN,void>
{
public:

	Delegate() {}
	Delegate(typename GlobalFunction<RETURN,void>::PTR pFunc) : DelegateBase(pFunc) {}

	template< class T >
	Delegate(T* pThis,RETURN (T::*pFunc)()) : DelegateBase(pThis,pFunc)
	{	
	}
	
	template< class T >
	Delegate(const T* pThis,RETURN (T::*pFunc)() const) : DelegateBase(pThis,pFunc)
	{
	}

	RETURN Call() const
	{
		return m_ptr->Call();
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
		DelegateMap::iterator iter = m_delegates.find(m_iId);

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

	/*
	data structures: map vs vector

	map: 
	
	cons - a bit slower traversal performance in Notifying the delegates because of the random memory locations,
	 but this could be solved by using a memory pool with a custom stl allocator. Logarithmic time for Attaching

	pros - Logarithmic time for detaching

	vector:

	cons - very slow in detaching delegates.
	pros - very fast at traversing, like a C array. Constant time Attaching delegates.

	*/

	typedef stdext::hash_map<int,DELEGATE> DelegateMap;

    DelegateMap m_delegates;
	int m_iId;
};

// Event with parameters
template < class RETURN , class PARAM >
class Event : public EventBase<RETURN,PARAM>
{
public:

	// Notify all delegates
    void Notify(PARAM a)
    {
        DelegateMap::iterator iter = m_delegates.begin();
        for(; iter != m_delegates.end(); iter++)
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

    void Notify()
    {
        DelegateMap::iterator iter = m_delegates.begin();
        for(; iter != m_delegates.end(); iter++)
        {
			iter->second.Call();
        }
    }
};

#endif // _DELEGATES_