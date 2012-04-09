#include "Heap.h"

using namespace std;
using namespace stdext;


Heap::Heap()
{
}
Heap::~Heap()
{
	// Save log file

	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		free(iter->second);
	}

}

// returns total memory usage of all memory pools
// this algorithm must be implemented this way, as in being 0(n) because
// the number of blocks each pool may change over time
unsigned int Heap::GetMemoryUsageInBytes() const
{
	unsigned int total = 0;
	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		const MemoryPool& pool = *(iter->second);
		total += pool.GetMemoryUsage();
	}

	return total;
}

void Heap::Sort()
{
	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		iter->second->SortBlocks();
	}
}

float Heap::GetMemoryUsageInKb() const
{
	float size = (float)GetMemoryUsageInBytes();
	return size / 1024.0f;
}

void Heap::GetMemoryInfo(char* pStr, unsigned int uiLength) const
{
	sprintf_s(pStr,uiLength,"\nTotal memory pool usage: %f(kb)\n",GetMemoryUsageInKb());
}

MemoryPool& Heap::GetPool(unsigned int size)
{
	size = ((size+7)&~7);

	auto iter = m_pool.find(size);
	
	if(iter == m_pool.end())
	{
		// alloc memory pool using malloc because the global version of new uses the memory pool,
		// which in turn calls this function.
		MemoryPool* pMemoryPool = (MemoryPool*)malloc(sizeof(MemoryPool));
		assert(pMemoryPool != nullptr);
		new (pMemoryPool) MemoryPool(size,(size >= 512 ? 5 : 512)); // construct memory pool

		// insert pool into hash map
		auto pair = m_pool.insert(make_pair(size,pMemoryPool)); // todo, could make size dynamic
		iter = pair.first;
	}

	// return the memory pool
	return *(iter->second);
}

/*void Heap::MemoryUsageChange(int bytes)
{
	m_uiMemoryUsage += bytes;
}*/
