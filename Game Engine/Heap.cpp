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
//!this algorithm must be implemented this way, as in being 0(n) because
//!the number of blocks each pool may change over time
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
	return GetMemoryUsageInBytes() / 1024.0f;
}

float Heap::GetMemoryUsageInMb() const
{
	return GetMemoryUsageInBytes() / 1048576.0f;
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
		// insert pool into hash map
		auto pair = m_pool.insert(make_pair(size,new MemoryPool(size,512))); // todo, could make size dynamic
		iter = pair.first;
	}

	// return the memory pool
	return *(iter->second);
}
