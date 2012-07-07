#include "Heap.h"
#include "VecMath.h"

using namespace std;
using namespace stdext;


Heap::Heap()
{
}
Heap::~Heap()
{
	// todo:Save log file

	// free memory
	for(auto iter = m_pool.begin(); iter != m_pool.end(); ++iter)
	{
		delete (iter->second);
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
	const unsigned int uiNewSize = ((size+7)&~7);
	const unsigned int uiDifference = uiNewSize - size;

	m_uiWastedBytes = uiDifference * 512;

	auto iter = m_pool.find(uiNewSize);
	
	if(iter == m_pool.end())
	{
		// insert pool into hash map
		//xtodo: could make size dynamic
		unsigned int uiBlocks = 512 - 2*uiNewSize;
		uiBlocks = Clamp(uiBlocks,16u,512u);


		auto pair = m_pool.insert(make_pair(uiNewSize,new MemoryPool(uiNewSize,uiBlocks)));
		iter = pair.first;
	}

	// return the memory pool
	return *(iter->second);
}
