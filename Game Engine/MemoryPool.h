/*
	Programmed by Bryce Mehring
	Starting Date: 1/24/2011
*/

#ifndef _MEMPOOL_
#define _MEMPOOL_


// global pool new and delete that use the MemoryPool
/*void* operator new(size_t size);
void operator delete(void* p);*/

class MemoryPool
{
public:

	/*

	if delete is not called on a object, memory will not be leaked, 
	only its destructor will not be called.

	In the constructor, the MemoryPool builds the linked list from 
	the contiguous memory block.

	When memory is needed, it is returned from the front of the list, then
	the node is removed from the list. If needed, another equally sized 
	memory block will be allocated and linked to the previous block.

	When the memory is deleted, it will be added to the front of the linked 
	list. 

	Each pool will have a pointer allocated at the beginning of each object
	to link it with the correct pool when deallocating.

	*/

	// constructors & destructors
	MemoryPool(unsigned int size, unsigned int n);
	~MemoryPool();

	void* Allocate();
	void Deallocate(void* p);

	// frees all memory in pool
	void FreePool();

	// these function should only be called during idle cpu time.
	// it sorts the pools so that the free pools are in front
	// The underlying sorting algorithm is insertion sort, returning the number of out of order nodes
	unsigned int SortBlocks();

	// info
	unsigned int GetBlocks() const;
	unsigned int GetSize() const;
	unsigned int GetLength() const;

	unsigned int GetMemoryUsage() const;

	void MemoryDump() const;

private:

	struct Node;

	struct MemoryBlock 
	{
		Node* pNode;

		// memory
		MemoryBlock* pNext;
	};

	struct Node
	{
		// The next free memory block in the list
		MemoryBlock* pMemoryBlock;

		unsigned int iSize; // number of MemoryBlocks in the list

		MemoryPool* pPool;

		// Pointers to create linked list of nodes
		Node* pNext;
		Node* pPrevious;
	};

	friend void BDelete(void* p);

	// ========== Data Members ========== 
	Node* m_pNode; // points to the current node
	Node* m_pNodeHead; // points to the first node
	Node* m_pNodeTail; // points to the last node

	unsigned int m_iSize; // sizeof(elements)
	const unsigned int m_iLength; // total number of elements in each memory block
	unsigned int m_iBlocks; // total number of blocks

	// =============== Helper Functions =============================


	// Checks if the current node has free space
	void CheckNextNodeForMemory();
	bool FreeSpace(Node* pNode) const; 

	// todo: the first two functions could be combined

	// for adding & moving nodes around
	void Move(Node* pTo, Node* pNode);
	void Erase(Node* pNode);
	void Insert(Node* pWhere, Node* pNode);

	// alloc memory block from the system
	char* AllocMemoryBlock();

	// creates a linked list from the contiguous memory block pMem
	// the amount of memory that pMem should be BlockArraySize()
	void LinkMemoryBlock();

	unsigned int BlockArraySize() const; // m_iLength * m_iSize

	// do not allow this class to be copied, yet
	MemoryPool(const MemoryPool& pool);
	MemoryPool operator =(const MemoryPool& pool);

};



#endif //_MEMPOOL_
