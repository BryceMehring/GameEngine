
#ifndef _MALLOCALLOCATOR_
#define _MALLOCALLOCATOR_

#include <limits>
#include <cassert>

	template <class T> class MallocAllocator;

	// specialize for void:
	 template <> class MallocAllocator<void> {
	public:
	  typedef void*       pointer;
	  typedef const void* const_pointer;
	  // reference to void members are impossible.
	  typedef void value_type;
	  template <class U> struct rebind { typedef MallocAllocator<U>
										 other; };
	};

	template <class T> class MallocAllocator {
	public:
	  typedef size_t    size_type;
	  typedef ptrdiff_t difference_type;
	  typedef T*        pointer;
	  typedef const T*  const_pointer;
	  typedef T&        reference;
	  typedef const T&  const_reference;
	  typedef T         value_type;
	  template <class U> struct rebind { typedef MallocAllocator<U>
										 other; };



	  MallocAllocator() throw() {}
	  MallocAllocator(const MallocAllocator&) throw() {}
	  template <class U> MallocAllocator(const MallocAllocator<U>&) throw() {}
	  ~MallocAllocator() throw() {}

	  pointer address(reference x) const { return &x; }
	  const_pointer address(const_reference x) const { return &x; }

	  pointer allocate(size_type n, MallocAllocator<void>::const_pointer hint = 0)
	  {
		 return (pointer)malloc(sizeof(T)*n);
	  }
	  void deallocate(pointer p, size_type n)
	  {
		   free(p);
	  }
	  size_type max_size() const throw() { return (size_type)(-1) / sizeof(T); }

	  void construct(pointer p, const T& val)
	  {
		  ::new ((void*)p) T(val);
	  }
	  void destroy(pointer p)
	  {
		  p->~T();
	  }

	};

	 template <class T1, class T2>
	 bool operator==(const MallocAllocator<T1>&, const MallocAllocator<T2>&)
				  throw()
	 {
		 return true;
	 }

	 template <class T1, class T2>
	 bool operator!=(const MallocAllocator<T1>&, const MallocAllocator<T2>&)
				  throw()
	 {
		 return false;
	 }

#endif //_MALLOCALLOCATOR_