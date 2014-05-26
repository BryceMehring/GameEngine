#ifndef _POOLEDALLOCATOR_
#define _POOLEDALLOCATOR_

#include "BNew.h"
#include <memory>
#include <cassert>

using namespace std;

// TEMPLATE CLASS _ALLOCATOR
// note: this allocator only works if the data structure only allocs
// note: one node at a time
template<class _Ty>
class PooledAllocator : public _Allocator_base<_Ty>
	{	// generic allocator for objects of class _Ty
public:
	typedef _Allocator_base<_Ty> _Mybase;
	typedef typename _Mybase::value_type value_type;

	typedef value_type _FARQ *pointer;
	typedef value_type _FARQ& reference;
	typedef const value_type _FARQ *const_pointer;
	typedef const value_type _FARQ& const_reference;

	typedef _SIZT size_type;
	typedef _PDFT difference_type;

	template<class _Other>
		struct rebind
		{	// convert this type to _ALLOCATOR<_Other>
		typedef PooledAllocator<_Other> other;
		};

	pointer address(reference _Val) const
		{	// return address of mutable _Val
		return ((pointer) &(char&)_Val);
		}

	const_pointer address(const_reference _Val) const
		{	// return address of nonmutable _Val
		return ((const_pointer) &(char&)_Val);
		}

	PooledAllocator() _THROW0()
		{	// construct default allocator (do nothing)
		}

	PooledAllocator(const _ALLOCATOR<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		PooledAllocator(const PooledAllocator<_Other>&) _THROW0()
		{	// construct from a related allocator (do nothing)
		}

	template<class _Other>
		PooledAllocator<_Ty>& operator=(const _ALLOCATOR<_Other>&)
		{	// assign from a related allocator (do nothing)
		return (*this);
		}

	void deallocate(pointer _Ptr, size_type)
		{	// deallocate object at _Ptr, ignore size
		::BDelete(_Ptr);
		}

	pointer allocate(size_type _Count)
		{	// allocate array of _Count elements
			assert(_Count  == 1);
		return (pointer)::Bnew(sizeof(_Ty));
		//return (_Allocate(_Count, (pointer)0));
		}

	pointer allocate(size_type _Count, const void _FARQ *)
		{	// allocate array of _Count elements, ignore hint
			assert(_Count  == 1);
		return ::Bnew(sizeof(_Ty));
		}

	void construct(pointer _Ptr, const _Ty& _Val)
		{	// construct object at _Ptr with value _Val
		_Construct(_Ptr, _Val);
		}

	void construct(pointer _Ptr, _Ty&& _Val)
		{	// construct object at _Ptr with value _Val
		::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Ty>(_Val));
		}

	template<class _Other>
		void construct(pointer _Ptr, _Other&& _Val)
		{	// construct object at _Ptr with value _Val
		::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Other>(_Val));
		}

	void destroy(pointer _Ptr)
		{	// destroy object at _Ptr
		_Destroy(_Ptr);
		}

	_SIZT max_size() const _THROW0()
		{	// estimate maximum array size
		_SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
		return (0 < _Count ? _Count : 1);
		}
	};

		// CLASS _ALLOCATOR<void>
template<> class PooledAllocator<void>
	{	// generic _ALLOCATOR for type void
public:
	typedef void _Ty;
	typedef _Ty _FARQ *pointer;
	typedef const _Ty _FARQ *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
		struct rebind
		{	// convert this type to an _ALLOCATOR<_Other>
		typedef _ALLOCATOR<_Other> other;
		};

	PooledAllocator() _THROW0()
		{	// construct default allocator (do nothing)
		}

	PooledAllocator(const PooledAllocator<_Ty>&) _THROW0()
		{	// construct by copying (do nothing)
		}

	template<class _Other>
		PooledAllocator(const PooledAllocator<_Other>&) _THROW0()
		{	// construct from related allocator (do nothing)
		}

	template<class _Other>
		PooledAllocator<_Ty>& operator=(const PooledAllocator<_Other>&)
		{	// assign from a related allocator (do nothing)
		return (*this);
		}
	};

template<class _Ty,
	class _Other> inline
	bool operator==(const PooledAllocator<_Ty>&,
		const PooledAllocator<_Other>&) _THROW0()
	{	// test for allocator equality
	return (true);
	}

template<class _Ty,
	class _Other> inline
	bool operator!=(const PooledAllocator<_Ty>& _Left,
		const PooledAllocator<_Other>& _Right) _THROW0()
	{	// test for allocator inequality
	return (!(_Left == _Right));
	}

#endif
