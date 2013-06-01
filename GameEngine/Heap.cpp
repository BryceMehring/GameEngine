#include "Heap.h"
#include "BNew.h"

using namespace std;

Heap::Heap() : m_uiAllocatedPools(0), MULTIPLE_OF(8), m_uiTotalWastedBytes(0)
{
	m_pool.resize(MAX_POOL_SIZE / MULTIPLE_OF);
}
Heap::~Heap()
{
	// todo:Save log file

	// free memory
	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		delete iter->pool;
	}

}

// returns total memory usage of all memory pools
//!this algorithm must be implemented this way, as in being 0(n) because
//!the number of blocks each pool may change over time
unsigned int Heap::GetMemoryUsageInBytes() const
{
	unsigned int total = 0;
	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		if(iter->pool != nullptr)
		{
			const MemoryPool& pool = *(iter->pool);
			total += pool.GetMemoryUsage();
		}
	}

	return total;
}

void Heap::Sort()
{
	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		if(iter->pool != nullptr)
		{
			iter->pool->SortBlocks();
		}
	}
}

void Heap::Compact()
{
	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		if(iter->pool != nullptr)
		{
			iter->pool->Compact();
		}
	}
}

float Heap::GetMemoryUsageInKb() const
{
	return GetMemoryUsageInBytes() / 1024.0f;
}

float Heap::GetMemoryUsageInMb() const
{
	return GetMemoryUsageInBytes() / 1048576.0f;
}


unsigned int Heap::GetNumberOfPools() const
{
	return m_uiAllocatedPools;
}

unsigned int Heap::GetWastedBytes(unsigned int pool) const
{
	return (pool >= GetNumberOfPools()) ? -1 : m_pool[pool].uiWastedBytes;
}
unsigned int Heap::GetWastedBytes() const
{
	return m_uiTotalWastedBytes;
}


MemoryPool& Heap::GetPool(unsigned int size)
{
	const unsigned int uiNewSize = ((size+(MULTIPLE_OF-1))&~(MULTIPLE_OF - 1));
	const unsigned int uiIndex = (uiNewSize / MULTIPLE_OF) - 1;
	
	if(m_pool[uiIndex].pool == nullptr)
	{
		// insert pool into vector
		const unsigned int uiBlocks = ((MAX_POOL_SIZE * 2) - uiNewSize);

		const unsigned int uiWastedBytes = uiBlocks*(uiNewSize - size);
		m_uiTotalWastedBytes += uiWastedBytes;

		m_pool[uiIndex].pool = new MemoryPool(uiNewSize,uiBlocks);
		m_pool[uiIndex].uiWastedBytes = uiWastedBytes;

		m_uiAllocatedPools++;
	}

	// return the memory pool
	return *m_pool[uiIndex].pool;
}
