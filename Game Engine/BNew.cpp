#include "BNew.h"
#include "Heap.h"
#include "MemoryPool.h"
#include "gassert.h"
#include "FileManager.h"

void* Bnew(unsigned int bytes)
{
	gassert(bytes <= MAX_POOL_SIZE,"Allocate less than 257 bytes\n from the memory pool");
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