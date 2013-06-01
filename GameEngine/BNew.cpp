#include "BNew.h"
#include "Heap.h"
#include "MemoryPool.h"

#include <cassert>

void* BNew(size_t bytes)
{
	assert(bytes <= MAX_POOL_SIZE);
	return Heap::Instance().GetPool(bytes).Allocate();
}

void BDelete(void* p)
{
	if(p != nullptr)
	{
		// release the memory back to the pool
		MemoryPool::MemoryBlock* pBlock = (MemoryPool::MemoryBlock*)((char*)p - sizeof(MemoryPool::Node*));
		pBlock->pNode->pPool->Deallocate(p);
	}
}
