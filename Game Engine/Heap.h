
#ifndef _HEAP_
#define _HEAP_

#include "MemoryPool.h"
#include "Singleton.h"
#include <vector>
#include <map>

class Heap : public Singleton<Heap>
{
public:

	friend class Singleton<Heap>;

	~Heap();

	// returns a memory pool with a size to a multiple of 8
	MemoryPool& GetPool(unsigned int size);

	// memory usage of all pools
	unsigned int GetMemoryUsageInBytes() const;
	float GetMemoryUsageInKb() const;
	float GetMemoryUsageInMb() const;

	unsigned int GetNumberOfPools() const;
	unsigned int GetWastedBytes(unsigned int pool) const;
	unsigned int GetWastedBytes() const;

	// Memory usage in text form
	void GetMemoryInfo(char* buffer, unsigned int uiLength) const;

	// todo: implement
	void SaveLogFile() const;

	void Sort();

private:

	Heap();

	struct Data
	{
		MemoryPool* pool;
		unsigned int uiWastedBytes;
	};

	// Data members
	typedef std::vector<Data> POOL_MAP;
	POOL_MAP m_pool;

	unsigned int m_uiTotalWastedBytes;
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