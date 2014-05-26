#ifndef _POOLEDALLOCATOR_
#define _POOLEDALLOCATOR_

#include "BNew.h"

template< class T >
class PooledAllocator
{
public:

	typedef T value_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	template< class U > struct rebind { typedef PooledAllocator<U> other; };

	PooledAllocator() {}
	PooledAllocator( const PooledAllocator& other ) {}

	template< class U > 
	PooledAllocator( const PooledAllocator<U>& other ) {}

	pointer address( reference x ) const
	{
		return &x;
	}

	const_pointer address( const_reference x ) const
	{
		return &x;
	}

	pointer allocate( size_type n, PooledAllocator<void>::const_pointer hint = 0 )
	{
		return (pointer)BNew(n * sizeof(T));
	}

	void deallocate( pointer p, size_type n )
	{
		BDelete(p);
	}

	size_type max_size() const
	{
		return std::numeric_limits<size_type>::max();
	}

	void construct( pointer p, const_reference val )
	{
		 new((void *)p) T(val);
	}

	void destroy( pointer p )
	{
		((T*)p)->~T();
	}
};

template<>
class PooledAllocator<void>
{
public:

	typedef void* pointer;
	typedef const void* const_pointer;
};

template< class T1, class T2 >
bool operator==( const PooledAllocator<T1>& lhs, const PooledAllocator<T2>& rhs )
{
	return true;
}

template< class T1, class T2 >
bool operator!=( const PooledAllocator<T1>& lhs, const PooledAllocator<T2>& rhs )
{
	return false;
}


#endif // _POOLEDALLOCATOR_