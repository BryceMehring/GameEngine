// Programmed by Bryce Mehring

#ifndef _BNEW_
#define _BNEW_

const unsigned int MAX_POOL_SIZE = 256;

// gets memory from the memory pool
// note: max size to get from pool is 256 bytes per object
void* Bnew(unsigned int bytes);

// return memory to the memory pool
void BDelete(void* p);

// macro version of class versions of new and delete
#define USE_MEM_POOL \
	public: \
		static void* operator new(unsigned int bytes) { return Bnew(bytes); } \
		static void  operator delete(void* p) { BDelete(p); }

// classes that use memory management should inherit from this class 
class Pooled
{
public:

	USE_MEM_POOL();

};

#endif