#include "BNew.h"
#include "Heap.h"
#include "MemoryPool.h"

void* operator new(size_t bytes)
{
	void* pMemory = nullptr;

	// only pool objects that are less than 1kb
	//if(bytes < 1024)
	{
		// get memory from pool
		Heap& heap = Heap::Instance();
		MemoryPool& pool = heap.GetPool(bytes);
		pMemory = pool.Allocate();

		// todo: I should mark the memory from the pool so that it does not
		// get treated as system memory
	}
	/*else
	{
		// Get memory from system
		pMemory = malloc(bytes+MEM_SIGNATURE_SIZE);
		assert(pMemory != nullptr);

		// Mark the first byte of memory
		SIGNATURE_DATA_TYPE* pTemp = (SIGNATURE_DATA_TYPE*)pMemory;
		*pTemp = MEM_SIGNATURE;

		pMemory = pTemp + 1;
	}
	*/
	return pMemory;
}
void operator delete(void* p)
{
	if(p != nullptr)
	{
		// If the memory is allocated from the system
		/*if( (*((SIGNATURE_DATA_TYPE*)(p)-1)) == MEM_SIGNATURE )
		{
			// free the memory
			free((SIGNATURE_DATA_TYPE*)(p)-1);
		}
		else*/
		{
			// release the memory back to the pool
			MemoryBlock* pBlock = (MemoryBlock*)((char*)p - sizeof(Node*));
			pBlock->pNode->pPool->Deallocate(p);
		}
	}
	
}