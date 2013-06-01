
#include "RefCounting.h"

using namespace std;

int RefCounting::s_iAddRef = 0;
int RefCounting::s_iRelease = 0;

RefCounting::RefCounting() : m_iCounter(1)
{
	++s_iAddRef;
}

int RefCounting::GetCounter() const { return m_iCounter; }

void RefCounting::AddRef()
{ 
	++s_iAddRef;
	++m_iCounter;
}

void RefCounting::Release()
{
	++s_iRelease;

	if(--m_iCounter == 0)
	{
		delete this;
	}
}

int RefCounting::GetAddRefCount()
{
	return s_iAddRef;
}

int RefCounting::GetAddReleaseCount()
{
	return s_iRelease;
}
