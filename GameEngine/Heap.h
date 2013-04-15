
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

	// todo: implement
	void SaveLogFile() const;

	void Sort();

	void Compact();

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

	unsigned int m_uiAllocatedPools;

	const unsigned int MULTIPLE_OF;

	unsigned int m_uiTotalWastedBytes;
};

#endif