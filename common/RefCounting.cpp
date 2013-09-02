
#include "RefCounting.h"

using namespace std;

RefCounting::RefCounting() : m_iCounter(1)
{
}

int RefCounting::GetCounter() const { return m_iCounter; }

void RefCounting::AddRef()
{ 
	++m_iCounter;
}

void RefCounting::Release()
{
	if(--m_iCounter == 0)
	{
		delete this;
	}
}