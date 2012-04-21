#include "BNew.h"
#include "Heap.h"
#include "MemoryPool.h"
#include <cassert>

void* Bnew(unsigned int bytes)
{
	assert(bytes <= 256);
	return Heap::Instance().GetPool(bytes).Allocate();
}

void BDelete(void* p)
{
	if(p != nullptr)
	{
		// release the memory back to the pool
		MemoryBlock* pBlock = (MemoryBlock*)((char*)p - sizeof(Node*));
		pBlock->pNode->pPool->Deallocate(p);
	}
}