
#ifndef _HEAP_
#define _HEAP_

#include "MemoryPool.h"
#include "MallocAllocator.h"
#include <hash_map>
#include <map>

class Heap
{
public:

	static Heap& Instance()
	{
		static Heap instance;
		return instance;
	}

	// returns a memory pool with a size to a multiple of 8
	MemoryPool& GetPool(unsigned int size);

	// memory usage of all pools in bytes
	unsigned int GetMemoryUsageInBytes() const;

	// memory usage of all pools in kilobytes
	float GetMemoryUsageInKb() const;

	// Memory usage in text form
	void GetMemoryInfo(char* pStr, unsigned int uiLength) const;

	// todo: implement
	void SaveLogFile() const;

	void Sort();

private:

	Heap();
	~Heap();

	// Data members
 
	typedef std::hash_map
	<unsigned int,MemoryPool*,
	stdext::hash_compare<unsigned int,std::less<unsigned int>>,
	MallocAllocator<std::pair<const unsigned int,
	MemoryPool*>>> POOL_MAP;

	POOL_MAP m_pool;

	// helper functions
	void MemoryUsageChange(int bytes);
};

/*#define MEMPOOL_DECLARE(CLASS) \
	public: \
		static void* operator new(size_t) \
		{ \
			Heap& heap = Heap::Instance(); \
			MemoryPool& pool = heap.GetPool(sizeof(CLASS)); \
			return pool.Allocate(); \
		} \
		static void operator delete(void* p) \
		{ \
			Heap& heap = Heap::Instance(); \
			MemoryPool& pool = heap.GetPool(sizeof(CLASS)); \
			pool.Deallocate(p); \
		} \*/

#endif