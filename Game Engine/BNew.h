#ifndef _BNEW_
#define _BNEW_

extern void* operator new(size_t bytes);
extern void operator delete(void* p);

#endif